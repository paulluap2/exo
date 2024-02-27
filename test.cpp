#include <iostream>

int main() {
    // Déclaration des variables pour stocker les nombres
    int nombre1, nombre2;

    // Demande à l'utilisateur de saisir le premier nombre
    std::cout << "Entrez le premier nombre : ";
    std::cin >> nombre1;

    // Demande à l'utilisateur de saisir le deuxième nombre
    std::cout << "Entrez le deuxième nombre : ";
    std::cin >> nombre2;

    // Calcule la somme des deux nombres
    int somme = nombre1 + nombre2;

    // Affiche la somme
    std::cout << "La somme de " << nombre1 << " et " << nombre2 << " est : " << somme << std::endl;

    return 0;
}
