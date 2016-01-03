// RecoFormGeoCpp.cpp : définit le point d'entrée pour l'application console.
/**
* Programme de reconnaissance de forme géometrique fait par Desfrançois Thomas et Manos Maxime
* Tutorial utilisé : http://www.nithinrajs.in/ocr-using-artificial-neural-network-opencv-part-1/
* 
* Ce programme a été fait pour être assez modulable, c'est à dire que le nombre de classe n'est pas définit dans le programme
* on peut très bien éssayer le programme avec des animeau par exemple, car les attributs sont trouvé en fonction de l'emplacement des pixels.
* Mais pour nos test nous avons utilisé des formes géométriques simple (une seule par image).
* Pour utiliser le programme : 
*	- Dans le dossier entrainement : créer un dossier par classe différente et y mettre les images corespondant à ses classes
*	- Dans le dossier test : créer un dossier par classe différente et y mettre les images corespondant à ses classes
*	-Dans le dossier prediction : il faut juste il mettre les images que l'on veut prédire
*
* ATTENTION : Pour prendre en compte des changements dans le dossier entrainement il faut absolument refaire un réseau de neuronnes avant de prédire
* Il y a aussi des images qui font planter le programme "le programme a cessé de fonctionner" la solution n'a pas été trouvé
*/

#include "stdafx.h"
#include "GeoReco.h"

/**
* Le main du programme permet de créer l'objet pour reconnaitre les formes
* géometriques
*/
int main()
{
	//L'objet permettant de créer et d'utiliser le réseau de neurones
	GeoReco geoReco;
	//Valeur pour selectionner un choix dans le menu
	int choix = -1;
	//Tant que l'utilisateur ne veut pas quitter
	while (choix != 0)
	{
		//Affichage du premier choix qui permet d'entrainer un réseau de neuronne
		std::cout << "1 - Pour creer le reseau de neuronnes" << std::endl;
		//Affichage du deuxième choix qui permet de tester les images dans le dossier prediction
		std::cout << "2 - Pour tester des images" << std::endl;
		//Affichage qu'il faut mettre 0 pour quitter
		std::cout << "0 - Pour quitter" << std::endl;
		//Récupération du choix
		std::cin >> choix;
		//En fonction du choix
		switch (choix)
		{
			case 1:
			{
				//Création du fichier d'entrainement
				geoReco.parcourirDossier(DOSSIER_ENTRAINEMENT, FICHIER_ENTRAINEMENT);//Parcourt les dossiers d'images, les transforme dans un fichier text, dernier caractère 0 à x pour carré, ect
				//Création du fichier de test
				geoReco.parcourirDossier(DOSSIER_TEST, FICHIER_TEST);
				//Entrainement du modele
				geoReco.entrainementModele();
				//Enregistrement du modele
				geoReco.enregistrerModele();
				//Permet de tester le nouveau modele
				geoReco.testModel(); 
				break;
			}
			case 2:
			{
				//Permet de charger le modèle à partir de fichier de sauvegarde
				geoReco.chargerModele(); 
				//Permet de predire les images dans le dossier de prediction
				geoReco.predireFormGeo();
				break;
			}
			case 0:
			{
				std::cout << "le programme quite" << std::endl;
				break;
			}
			default:
			{
				std::cout << "Mauvaise commande" << std::endl;
			}
		}
	}
	return 0;
}