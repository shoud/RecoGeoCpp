#pragma once
#include <opencv2/core/core.hpp>
#include <opencv2/ml/ml.hpp>


#ifdef _WIN32
#define PATH_SEPARATOR "\\"
#else
#define PATH_SEPARATOR "/"
#endif

#define DOSSIER_ENTRAINEMENT "entrainement"
#define DOSSIER_TEST "test"
#define DOSSIER_PREDICTION "prediction"
#define FICHIER_ENTRAINEMENT "entrainement.att"
#define FICHIER_TEST "test.att"
#define FICHIER_CLASSE "classes.cla"
#define FICHIER_MODELE "modele.mdl"

class GeoReco
{
	private:
		//La liste des noms des classes
		std::vector<std::string> listeClasses;
		//Le nombre de classes
		int nbClasses;
		//Pour redimension les images à la même taille
		int dimension;
		//Le nombre d'attributs par images
		int attributs;
		//Nombre d'images d'entrainement
		int nombreImageEntrainement;
		//Nombre d'images pour le test
		int nombreImagetest;
		//Le modele de réseau de neuronnes
		cv::Ptr<cv::ml::ANN_MLP> modele;
		//La matrice d'attributs pour l'entrainement
		cv::Mat matriceEntrainementAttributs;
		//La matrice de classe pour l'entrainement
		cv::Mat matriceEntrainementClasses;
		//La matrice d'attributs pour les tests
		cv::Mat matriceTestAttributs;
		//La matrice de classe pour les tests
		cv::Mat matriceTestClasses;
		//Permet de convertire un fichier en matrice d'attributs et de classes
		void fichierEnMatrices(std::string, cv::Mat&, cv::Mat&);
		//Permet de rogner une image
		void rognerImage(cv::Mat&);
		//Permet de mettre à jour le nombre de classes
		void nombreDeClasses();
		//Permet de mettre à jour la liste de classes
		void listerClasses();
		//Renvoie le nombre de lignes dans un fichier
		int nombreDeLignes(std::string path);
		
	public:
		//Constructeur initialise la taille des image à traiter
		GeoReco();
		//Fonction permettant de récupérer les attributs des images d'un dossier représentant une classe
		void recuperationAttributs(std::string, std::string, std::string);
		//Permet de créer le réseau de neuronnes
		void entrainementModele();
		//Permet de tester le modele en donnant un pourcentage de réussite
		void testModel();
		//Permet de prédire des images
		void predireFormGeo();
		//Permet de parcourir les dossiers représentant des classes
		void parcourirDossier(std::string, std::string);
		//Permet d'enregistrer le modele
		void enregistrerModele();
		//Permet de charger le modele
		void chargerModele();
};
