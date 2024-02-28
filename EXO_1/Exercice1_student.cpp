#include <iostream>       // basic input output streams
#include <fstream>        // input output file stream class
#include <cmath>          // librerie mathematique de base
#include <iomanip>        // input output manipulators
#include <valarray>       // valarray functions
#include "ConfigFile.h" // Il contient les methodes pour lire inputs et ecrire outputs 
                          // Fichier .tpp car inclut fonctions template
#include <numeric>

using namespace std; // ouvrir un namespace avec la librerie c++ de base

/* La class Engine est le moteur principale de ce code. Il contient 
   les methodes de base pour lire / initialiser les inputs, 
   preparer les outputs et calculer les donnees necessaires
*/
class Engine
{
private:
    // Existing private members of Engine...
  const double pi=3.1415926535897932384626433832795028841971e0;

// EngineEuler specific members
  unsigned int maxit; // nombre maximale d iterations
  double tol;        // tolerance methode iterative
  double alpha;        // parametre pour le scheme d'Euler

  // definition des variables
  double tfin;         // Temps final
  unsigned int nsteps; // Nombre de pas de temps
  double mu; 	       // paramètre de Magnus
  double mass;         // masse de la balle
  double R;            // rayon de la balle
  double omega;        // vitesse de rotation de la balle
  double rho;          // densité du fluide
  double g;            // accélération de la pesanteur
  double Ct;           // coefficient de trainée aérodynamique 
  double S;            // surface de section de la balle (pi * R^2) 
  double Om;           // variable auxiliaire

  valarray<double> y0 = std::valarray<double>(0.e0, 4); // Correctly initialized
  valarray<double> y  = std::valarray<double>(0.e0, 4); // Correctly initialized

  double t,dt;  // Temps courant pas de temps

  unsigned int sampling;  // Nombre de pas de temps entre chaque ecriture des diagnostics
  unsigned int last;       // Nombre de pas de temps depuis la derniere ecriture des diagnostics
  ofstream *outputFile;    // Pointeur vers le fichier de sortie

  /* Calculer et ecrire les diagnostics dans un fichier
     inputs:
     write: (bool) ecriture de tous les sampling si faux
  */  
  void printOut(bool write)
  {
  // TODO calculer l'energie mecanique
    double Energy = (1.0/2.0)*mass*(y[2]*y[2]+y[3]*y[3])+(1.0/5.0)*mass*R*R*omega*omega+mass*g*y[1];

    // Ecriture tous les [sampling] pas de temps, sauf si write est vrai
    if((!write && last>=sampling) || (write && last!=1))
    {
      *outputFile << t << " " << y[0] << " " << y[1] << " " \
      << y[2] << " " << y[3] << " " << Energy<< endl; // write output on file
      last = 1;
    }
    else
    {
      last++;
    }
  }

  // TODO écrire la fonction 
    void compute_f(valarray<double>& f) const
    {
      double Frict = 0.0;
      
      f[0]      = y[2];
      f[1]      = y[3];
      f[2]      = -mu*R*R*R*rho*omega*y[3]/mass;
      f[3]      = (mu*R*R*R*rho*omega*y[2]/mass)-g;
    }

    // New step method from EngineEuler
    void step()
    {
      unsigned int iteration=0;
      double error=999e0;
      valarray<double> f =valarray<double>(0.e0,4); 
      valarray<double> yold=valarray<double>(y);
      valarray<double> delta_y_EE=valarray<double>(y);
      valarray<double> fyold =valarray<double>(0.e0,4);
      compute_f(fyold);

      if(alpha >= 0. && alpha <= 1.0){
      // TODO écrire l'algorithme qui peut être explicite, implicite ou semi-implicite d'Euler en variant alpha 
      y = yold;
      compute_f(f);
      do{
		  y = yold + (alpha*fyold+(1-alpha)*f)*dt;
		  compute_f(f);
		  delta_y_EE = abs(y -yold - (alpha*fyold+(1-alpha)*f)*dt);
		  error = sqrt(inner_product(begin(delta_y_EE),end(delta_y_EE),begin(delta_y_EE),0.0));
		  ++iteration;
		}while((error >= tol)and(iteration <= maxit));
      }
      else
      {
        cerr << "alpha not valid" << endl;
      }
      t+=dt;
    }

    void EulerExplicite(){
      valarray<double> f =valarray<double>(0.e0,4);
      compute_f(f);
      y = y + f*dt;
      t+=dt;
    }
    void EulerImplicite(){
      unsigned int iteration=0;
      double error=999e0;
      valarray<double> f =valarray<double>(0.e0,4); 
      valarray<double> yold=valarray<double>(y);
      valarray<double> y_control=valarray<double>(y);
      valarray<double> delta_y_EE=valarray<double>(y);

      y = yold;
      compute_f(f);
      int k = 0;
      do{
        cout<<"avant: "<<y[0]<<", "<<y[1]<<", "<<y[2]<<", "<<y[3]<<endl;
		  y = yold + f*dt;
        cout<<"après: "<<y[0]<<", "<<y[1]<<", "<<y[2]<<", "<<y[3]<<endl;
      compute_f(f);
      y_control = yold + f*dt;
       cout << "ycontrol: "<<y_control[0]<<", "<<y_control[1]<<", "<<y_control[2]<<", "<<y_control[3]<<endl;
		  delta_y_EE = abs(y - y_control);
      cout << "delta: "<<delta_y_EE[0]<<", "<<delta_y_EE[1]<<", "<<delta_y_EE[2]<<", "<<delta_y_EE[3]<<endl;
		  error = sqrt(inner_product(begin(delta_y_EE),end(delta_y_EE),begin(delta_y_EE),0.0));
      cout << "k = " << k << " et error = " << error << endl;
		  ++iteration;
      ++k;
		}while((error >= tol)and(iteration <= maxit));
    
      t+=dt;
    }
public:
    // Modified constructor
    Engine(ConfigFile configFile)
    {
      // Stockage des parametres de simulation dans les attributs de la classe
      tfin     = configFile.get<double>("tfin",tfin);	        // lire le temps final de simulation
      nsteps   = configFile.get<unsigned int>("nsteps",nsteps); // lire le nombre de pas de temps
      y0[0]    = configFile.get<double>("x0",y0[0]);  // position initiale selon x	    
      y0[1]    = configFile.get<double>("y0",y0[1]);  // position initiale selon y       
      y0[2]    = configFile.get<double>("vx0",y0[2]); // vitesse initiale selon x       
      y0[3]    = configFile.get<double>("vy0",y0[3]); // vitesse initiale selon y	    
      mass     = configFile.get<double>("mass",mass);           
      g        = configFile.get<double>("g",g);           
      omega    = 2*pi*configFile.get<double>("omega",omega);     //omega est en rad/s  
      mu       = configFile.get<double>("mu",mu);            
      R        = configFile.get<double>("R",R);            
      rho      = configFile.get<double>("rho",rho);        
      Ct       = configFile.get<double>("Ct",Ct);        
      sampling = configFile.get<unsigned int>("sampling",sampling);
      tol      = configFile.get<double>("tol", tol);
      maxit    = configFile.get<unsigned int>("maxit", maxit);
      alpha    = configFile.get<double>("alpha", alpha);
      dt       = tfin / nsteps; // calculer le time step

      // Ouverture du fichier de sortie
      outputFile = new ofstream(configFile.get<string>("output","output.out").c_str()); 
      outputFile->precision(15); // Les nombres seront ecrits avec 15 decimales
    };


    // Destructeur virtuel
    virtual ~Engine()
    {
      outputFile->close();
      delete outputFile;
    };
      // Simulation complete
    void run()
    {
      // TODO à ajuster selon vos besoins
      S  = pi*R*R;
      Om = 0.0; 
      t  = 0.e0; // initialiser le temps
      y  = y0;   // initialiser
      last = 0; // initialise le parametre d'ecriture
      printOut(true); // ecrire la condition initiale

      for(unsigned int i(0); i<nsteps; ++i) // boucle sur les pas de temps
      {
      step();  // faire un pas de temps
      printOut(false); // ecrire le pas de temps actuel
      }
      printOut(true); // ecrire le dernier pas de temps
    };
};

// programme
int main(int argc, char* argv[])
{
  // Existing main function implementation
  // ...
  string inputPath("configuration.in.example"); // Fichier d'input par defaut
  if(argc>1) // Fichier d'input specifie par l'utilisateur ("./Exercice2 config_perso.in")
      inputPath = argv[1];

  ConfigFile configFile(inputPath); // Les parametres sont lus et stockes dans une "map" de strings.

  for(int i(2); i<argc; ++i) // Input complementaires ("./Exercice2 config_perso.in input_scan=[valeur]")
      configFile.process(argv[i]);

  Engine* engine;

  // Create an instance of Engine instead of EngineEuler
  engine = new Engine(configFile);

  engine->run(); // executer la simulation

  delete engine; // effacer la class simulation 
  cout << "Fin de la simulation." << endl;
  return 0;
}



    
