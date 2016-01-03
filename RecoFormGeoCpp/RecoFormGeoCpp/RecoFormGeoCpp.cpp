// RecoFormGeoCpp.cpp�: d�finit le point d'entr�e pour l'application console.
/**
* Programme de reconnaissance de forme g�ometrique fait par Desfran�ois Thomas et Manos Maxime
* Tutorial utilis� : http://www.nithinrajs.in/ocr-using-artificial-neural-network-opencv-part-1/
* 
* Ce programme a �t� fait pour �tre assez modulable, c'est � dire que le nombre de classe n'est pas d�finit dans le programme
* on peut tr�s bien �ssayer le programme avec des animeau par exemple, car les attributs sont trouv� en fonction de l'emplacement des pixels.
* Mais pour nos test nous avons utilis� des formes g�om�triques simple (une seule par image).
* Pour utiliser le programme : 
*	- Dans le dossier entrainement : cr�er un dossier par classe diff�rente et y mettre les images corespondant � ses classes
*	- Dans le dossier test : cr�er un dossier par classe diff�rente et y mettre les images corespondant � ses classes
*	-Dans le dossier prediction : il faut juste il mettre les images que l'on veut pr�dire
*
* ATTENTION : Pour prendre en compte des changements dans le dossier entrainement il faut absolument refaire un r�seau de neuronnes avant de pr�dire
* Il y a aussi des images qui font planter le programme "le programme a cess� de fonctionner" la solution n'a pas �t� trouv�
*/

#include "stdafx.h"
#include "GeoReco.h"

/**
* Le main du programme permet de cr�er l'objet pour reconnaitre les formes
* g�ometriques
*/
int main()
{
	//L'objet permettant de cr�er et d'utiliser le r�seau de neurones
	GeoReco geoReco;
	//Valeur pour selectionner un choix dans le menu
	int choix = -1;
	//Tant que l'utilisateur ne veut pas quitter
	while (choix != 0)
	{
		//Affichage du premier choix qui permet d'entrainer un r�seau de neuronne
		std::cout << "1 - Pour creer le reseau de neuronnes" << std::endl;
		//Affichage du deuxi�me choix qui permet de tester les images dans le dossier prediction
		std::cout << "2 - Pour tester des images" << std::endl;
		//Affichage qu'il faut mettre 0 pour quitter
		std::cout << "0 - Pour quitter" << std::endl;
		//R�cup�ration du choix
		std::cin >> choix;
		//En fonction du choix
		switch (choix)
		{
			case 1:
			{
				//Cr�ation du fichier d'entrainement
				geoReco.parcourirDossier(DOSSIER_ENTRAINEMENT, FICHIER_ENTRAINEMENT);//Parcourt les dossiers d'images, les transforme dans un fichier text, dernier caract�re 0 � x pour carr�, ect
				//Cr�ation du fichier de test
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
				//Permet de charger le mod�le � partir de fichier de sauvegarde
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