#include "stdafx.h"
#include "GeoReco.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>

/**
* Constructeur de la classe GeoReco, il permet d'initialiser la taille
* des images pour le traitement
*/
GeoReco::GeoReco()
{
	//La dimension pour normaliser toutes les images
	this->dimension = 32;
	//Le nombre d'attribues 
	attributs = dimension*dimension;
	//Initialisation du nombre de classe � zero
	nbClasses = -1;
}

/**
* Permet de rogner l'image pour garder le plus important
* (trouv� sur internet)
*/
void GeoReco::rognerImage(cv::Mat& img)
{
	std::vector<cv::Point> pixels;
	pixels.reserve(img.rows*img.cols);

	for (int j = 0; j < img.rows; ++j) {
		for (int i = 0; i < img.cols; ++i) {
			if (img.at<uchar>(j, i) != 255)
				pixels.push_back(cv::Point(i, j));
		}
	}

	cv::Rect crop = cv::boundingRect(pixels);
	img = img(crop);
}

/**
* Permet de parcourir tout les dossier repr�sentant une classe
* Met � jour le fichier contenant le nom des diff�rentes classes
*/
void GeoReco::parcourirDossier(std::string dossier, std::string fichierSortie)
{
	//Permet de supprimer l'ancienne version du fichier contenant les attributs
	std::ofstream fichier(fichierSortie);
	//Cr�ation du fichier contenant les classes
	std::ofstream classeFile(FICHIER_CLASSE);
	fichier.close();
	//Ouverture du dossier contenant les dossiers contenant les donn�es par classes
	DIR * rep = opendir((char *)dossier.c_str());
	if (rep != NULL)
	{
		struct dirent * ent;
		//On parcout tout les fichiers (dossiers)
		while ((ent = readdir(rep)) != NULL)
		{
			std::string nom = ent->d_name;
			//Pour chaques classes diff�rentes (cercle, rectangle, etc ..)
			if ((nom.compare(".") != 0) && (nom.compare("..") != 0))
			{
				//Une classe en plus
				nbClasses++;
				//Ecriture du nom de la classe dans le fichier de classes
				classeFile << nom << '\n';
				//R�cup�ration des attributs de toutes les images du dossier
				recuperationAttributs((dossier + PATH_SEPARATOR + nom), nom, fichierSortie);
			}
		}
		//Fermeture du fichier contenant les classes
		classeFile.close();
		//Fermeture du dossier
		closedir(rep);
	}
}

/**
* Fonction permettant de r�cup�rer les attributs des images d'un dossier repr�sentant une classe
*/
void GeoReco::recuperationAttributs(std::string dossier, std::string classe, std::string fichierSortie)
{
	//Permet d'ouvrir ou de cr�er le fichier de sortie (entrainement ou test)
	std::ofstream file(fichierSortie, std::ofstream::out | std::ofstream::app);
	//Ouverture du dossier contenant les images d'entrainement ou de test
	DIR * rep = opendir((char*)dossier.c_str());
	//Si le dossio existe
	if (rep != NULL)
	{
		struct dirent * ent;
		//Parcourt de toute les images
		while ((ent = readdir(rep)) != NULL)
		{
			//Recuperation du nom de l'image courante
			std::string nom = ent->d_name;
			//Ne pas prendre le retour en arri�re
			if ((nom.compare(".") != 0) && (nom.compare("..") != 0))
			{
				//Le chemin de l'image a r�cup�rer
				std::string cheminImage = dossier + PATH_SEPARATOR + nom;
				//Lecture et enregistrement de l'image dans une matrice  
				cv::Mat image = cv::imread(cheminImage, 0);
				//Pour mettre l'image en noir et blanc
				if (image.type() != CV_8UC1)
					cv::cvtColor(image, image, CV_BGR2GRAY);
				//Rend floue l'image
				cv::GaussianBlur(image, image, cv::Size(5, 5), 0);
				//Rend net l'image
				cv::threshold(image, image, 100, 255, 0);
				//Rogne l'image
				rognerImage(image);
				//Si l'image n'est pas devenue vide
				if (image.size() != cv::Size(0,0))
				{
					//Cr�ation de la matrice pour r�cup�rer l'image redimensionn�
					cv::Mat imageRedimension(dimension, dimension, CV_16U, cv::Scalar(0));
					//Redimensionnement de l'image au format 32*32
					resize(image, imageRedimension, imageRedimension.size());
					//Pour chaque pixel de l'image
					for (int x = 0; x < dimension; ++x)
						for (int y = 0; y < dimension; ++y)
								//Enregistrement dans le fichier entrainement ou test
								file << ((imageRedimension.at<uchar>(x, y) == 255) ? 1 : 0);
					//La derni�re valeur permmet de d�finir sa classe
					file << nbClasses << '\n';
				}
			}
		}
		//Fermeture du r�pertoire
		closedir(rep);
	}
	//fermeture du fichier
	file.close();
}

/**
* Fonction permettant de conna�tre le nombre de ligne dans un fichier
*/
int GeoReco::nombreDeLignes(std::string path)
{
	//Ouverture en lecture du fichier
	std::ifstream file(path);
	//Pour r�cup�rer la ligne courante
	std::string ligne;
	//Initialisation du nombre de ligne � 0
	int nbLignes = 0;
	//Lecture de chaque ligne
	while (std::getline(file, ligne))
		//Comptage de la ligne
		nbLignes++;
	//Fermeture du fichier
	file.close();
	//Retourne le nombre de ligne dans le fichier
	return nbLignes;
}

/**
* Fonction permettant de tranformer un fichier d'attributs en matrices 
*/
void GeoReco::fichierEnMatrices(std::string chemin, cv::Mat& matriceAttributs, cv::Mat& matriceClasses) {
	//Ouverture en lecture du fichier contenant les attributs
	std::ifstream fichier(chemin);
	//Si le fichier est bien ouvert
	if(fichier.is_open())
	{
		//Initialisation des lignes � 0
		int i = 0;
		//Pour garder la ligne courante
		std::string ligne;
		//Pour chaque ligne (img)
		while (getline(fichier, ligne))
		{
			int j = 0;
			//Pour chaque pixel
			for (; j < attributs; j++)
				//On met chaque pixel dans la matrice d'attributs
				matriceAttributs.at<float>(i, j) = (ligne[j] == '1' ? 1.f : 0.f);
			//i = ligne , colonne : carr�, cercle
			matriceClasses.at<float>(i++, stoi(ligne.substr(j))) = 1.f;
		}
	}
	//Ferme le fichier
	fichier.close();
}

/**
* Permet de compter le nombre de classes
*/
void GeoReco::nombreDeClasses()
{
	//Compte le nombre de ligne et donc le nombre de classe dans le fichier contenant les classes
	nbClasses = nombreDeLignes(FICHIER_CLASSE);
}

/**
* Fonction permettant de cr�er le r�seau de neuronne
*/
void GeoReco::entrainementModele()
{
	//R�cup�ration du nombre de classes
	nombreDeClasses();
	//R�cup�ration du nombre d'image retenues pour l'entrainement
	nombreImageEntrainement = nombreDeLignes(FICHIER_ENTRAINEMENT);
	//Cr�ation de la matrice d'entrainement
	matriceEntrainementAttributs = cv::Mat(nombreImageEntrainement, attributs, CV_32F);
	//Cr�ation de matrice de classes
	matriceEntrainementClasses = cv::Mat(nombreImageEntrainement, nbClasses, CV_32F);
	//Envoie des deux matrices vides � traiter pour les mettres � jour en fonction du fichier
	fichierEnMatrices(FICHIER_ENTRAINEMENT, matriceEntrainementAttributs, matriceEntrainementClasses);
	cv::Mat layers(3, 1, CV_32S);
	layers.at<int>(0, 0) = attributs;
	layers.at<int>(1, 0) = 16;
	layers.at<int>(2, 0) = nbClasses;
	modele = cv::ml::ANN_MLP::create();
	modele->setLayerSizes(layers);
	modele->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM, 0.6, 1);
	modele->setTermCriteria(cv::TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 1000, 0.000001));
	modele->setTrainMethod(cv::ml::ANN_MLP::BACKPROP, 0.1, 0.1);
	cv::Ptr<cv::ml::TrainData> entrainementIndividus = cv::ml::TrainData::create(matriceEntrainementAttributs, cv::ml::ROW_SAMPLE, matriceEntrainementClasses);
	//Entraine le r�seau de neuronne
	modele->train(entrainementIndividus);
}

/**
* Permet de tester le modele, affiche le % de r�ussite
*/
void GeoReco::testModel()
{
	//Recuperation du nombre de classes
	nombreDeClasses();
	//Le bon nombre d'image de test
	nombreImagetest = nombreDeLignes(FICHIER_TEST);
	//Cr�ation d'une matrice vide pour contenir les attributs
	matriceTestAttributs = cv::Mat(nombreImagetest, attributs, CV_32F);
	//Cr�ation d'une matrice vide pour contenir les classes
	matriceTestClasses = cv::Mat(nombreImagetest, nbClasses, CV_32F);
	//Met � jour les matrices
	fichierEnMatrices(FICHIER_TEST, matriceTestAttributs, matriceTestClasses);
	//Le nombre de fois o� il a trouv� la bonne classe
	int reussit = 0; 
	//Le num�ro de la meilleur classe trouv�e
	int meilleurClasse;
	//La valeur repr�sentant le r�sultat donn�e en fonction de la classe
	float valeur;
	//La meilleur valeur trouv�e
	float meilleurValeur;
	//Matrice contenant l'image � traiter
	cv::Mat image;
	//Matrice contenant les r�sultats
	cv::Mat	matriceResultat(1, nbClasses, CV_32F);
	//Parcout toutes les images de test
	for (int i = 0; i < nombreImagetest; ++i)
	{
		//R�cup�ration d'une seule image
		image = matriceTestAttributs.row(i);
		//Permet de calculer la solution
		modele->predict(image, matriceResultat);
		//Initialisation de la meilleur classe � 0
		meilleurClasse = 0;
		//Intialisation de la valeur � 0
		valeur = 0;
		//R�cup�ration de la premi�re valeur de la matrice
		meilleurValeur = matriceResultat.at<float>(0, 0);
		//Parcourt de toutes les classes de la matrice
		for(int numeroClasse = 0; numeroClasse < nbClasses; ++numeroClasse)
		{
			//R�cup�ration de la valeur (il ya qu'une ligne, les classes sont les colonnes)
			valeur = matriceResultat.at<float>(0, numeroClasse);
			//Si la valeur courante et plus grande que la meilleur valeur
			if (valeur > meilleurValeur)
			{
				//Mise � jour de la meilleur valeur
				meilleurValeur = valeur;
				//R�cup�ration de la meilleur classe acutel
				meilleurClasse = numeroClasse;
			}
		}
		//Permet de v�rifier le resultat
		if(matriceTestClasses.at<float>(i, meilleurClasse) == 1.f)
			//Rajoute une r�ussite si le r�sultat trouv� et le bon
			++reussit;
	}
	//Afficher le pourcentage de r�ussite
	double pourcentageReussite = reussit * 100.f / nombreImagetest;
	std::cout << "Pourcentage de reussite : " << pourcentageReussite << std::endl;
}

/**
* Permet de pr�dire l'enssemble des images se trouvant dans le dossier de prediction
*/
void GeoReco::predireFormGeo()
{
	//Recuperation du dossier de prediction car pause un probl�me quand deux define � la suite
	std::string dossier = DOSSIER_PREDICTION;
	//Recuperation du nombre de classes
	nombreDeClasses();
	//Ouverture du dossier contenant les dossiers contenant les images � predire
	DIR * rep = opendir(DOSSIER_PREDICTION);
	//Si le dossier existe et lisible
	if (rep != NULL)
	{
		struct dirent * ent;
		//On parcout toutes les images
		while ((ent = readdir(rep)) != NULL)
		{
			//Recup�ration du nom de l'image
			std::string nom = ent->d_name;
			//Pour eviter le dossier de retour, etc
			if ((nom.compare(".") != 0) && (nom.compare("..") != 0))
			{
				//Le num�ro de la classe courante
				int numeroClasse;
				//Le num�ro de la meilleur classe
				int meilleurClasse;
				//La valeur courante
				float valeur;
				//La meilleur valeur
				float meilleurValeur;
				//Recup�ration et stockage de l'image dans une matrice
				cv::Mat image = cv::imread(dossier + PATH_SEPARATOR + nom, 0);
				//Permet de mettre l'image en noir et blanc, � rajouter dans entrainement
				if (image.type() != CV_8UC1)
					cv::cvtColor(image, image, CV_BGR2GRAY);
				//Mettre en flou
				cv::GaussianBlur(image, image, cv::Size(5, 5), 0);
				//Mettre en net
				cv::threshold(image, image, 100, 255, 0); //50
				//Permet de rogner l'image
				rognerImage(image);
				//Si l'image n'est pas devenue vide
				if (image.size() != cv::Size(0, 0))
				{
					//Matrice contenant l'image redimensionn�e
					cv::Mat imageRedimensionner(dimension, dimension, CV_16U, cv::Scalar(0));
					//Redimensionnement de l'image
					resize(image, imageRedimensionner, imageRedimensionner.size());
					//Matrice contenant les attributs de l'images
					cv::Mat matriceAttributs(1, attributs, CV_32F);
					//Matrice repr�sentant les classes
					cv::Mat	matriceClasse(1, nbClasses, CV_32F);
					int j = 0;
					//Pour chaque pixel
					for(int x = 0; x < dimension; ++x)
						for (int y = 0; y < dimension; ++y)
							//Mise � jour de la matrice d'attributs
							matriceAttributs.at<float>(0, j++) = ((imageRedimensionner.at<uchar>(x, y) == 255) ? 1 : 0);
					//Met � jour la matrice de classes gr�ce � la matrice d'attributs
					modele->predict(matriceAttributs, matriceClasse);
					//Initialisation de la meilleur classe � 0
					meilleurClasse = 0;
					//Initialisation de la meilleur valeur avec la premi�re valeur de la matrice de r�sulat (classe)
					meilleurValeur = matriceClasse.at<float>(0, 0);
					//Mise � jour de la liste de classes permet de traduire le num�ro de la classe en nom de la classe
					listerClasses();
					//Parcourt de toutes la valeurs (= au nombre de classes)
					for(numeroClasse = 0; numeroClasse < nbClasses; ++numeroClasse)
					{
						//R�cup�ration de la valeur par rapport � la classe
						valeur = matriceClasse.at<float>(0, numeroClasse);
						//Affichage de la valeur pour la classe
						std::cout << listeClasses[numeroClasse] << " = " << valeur << std::endl;
						//Si la valeur courante et meilleur que la meilleur valeur
						if(valeur > meilleurValeur)
						{
							//Mise � jour de la meilleur valeur
							meilleurValeur = valeur;
							//Mise � jour de la meilleur classe
							meilleurClasse = numeroClasse;
						}
					}
					//Affichage du r�sultat
					std::cout << "L'image " << nom  << " est un -> " << listeClasses[meilleurClasse] << std::endl;
					//Permet de sauter une ligne quand plusieurs image � predire
					std::cout << std::endl;
				}
				else
					//Si il y a un probleme avec l'image
					std::cout << "L'image n'a pas pu etre traiter " << std::endl;
			}
		}
		//Fermeture du dossier des images � predire
		closedir(rep);
	}
}
/**
* Met � jour la liste des classes gr�ce au fichier de classe
*/
void GeoReco::listerClasses()
{
	//Suppression de la liste courante
	listeClasses.clear();
	//Ouverture du fichier contenant les classes
	std::ifstream file(FICHIER_CLASSE);
	//Pour r�cup�rer le nom de la classe
	std::string ligne;
	//Lecture ligne par ligne
	while (std::getline(file, ligne))
		//Rajout de la classe dans la liste
		listeClasses.push_back(ligne);
	//Fermeture du fichier contenant les classes
	file.close();
}

/**
* Permet d'enregistrer dans un fichier le modele
*/
void GeoReco::enregistrerModele()
{
	//Enregistre le modele dans un fichier
	modele->save(FICHIER_MODELE);
}

/**
* Permet de charger un modele � partir d'un fichier
*/
void GeoReco::chargerModele()
{
	//Charge le modele � partir d'un fichier
	modele = cv::ml::StatModel::load<cv::ml::ANN_MLP>(FICHIER_MODELE);
}