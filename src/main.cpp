#include <stdio.h>
#include <proj.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

//Guerledan_Feb19_50cm_wgs84.txt

double* projection(double longi,double lat,double z1,double z2) 
{
    PJ_CONTEXT *C;
    PJ *P;
    PJ* P_for_GIS;
    PJ_COORD a, b;

    double *err;
    double val_err = 1.0;
    err = &val_err;

    C = proj_context_create();

    P = proj_create_crs_to_crs (C,
                                "EPSG:4326", /* WGS84 */
                                "EPSG:2154", /* Lambert93 */
                                NULL);

    if (0==P) {
        fprintf(stderr, "Oops\n");
        return err;
    }

    /* This will ensure that the order of coordinates for the input CRS */
    /* will be longitude, latitude, whereas EPSG:4326 mandates latitude, */
    /* longitude */
    P_for_GIS = proj_normalize_for_visualization(C, P);
    if( 0 == P_for_GIS )  {
        fprintf(stderr, "Oops\n");
        return err;
    }
    proj_destroy(P);
    P = P_for_GIS;

    /* a coordinate union representing Copenhagen: 55d N, 12d E    */
    /* Given that we have used proj_normalize_for_visualization(), the order of
    /* coordinates is longitude, latitude, and values are expressed in degrees. */
    //a = proj_coord (12, 55, 0, 0);
    a = proj_coord (longi, lat, z1, z2);

    /* transform to Lamber93 */
    b = proj_trans (P, PJ_FWD, a);
    //printf ("easting: %.3f, northing: %.3f\n", b.enu.e, b.enu.n);
    
    double coord[2];
    coord[0] = b.enu.e;
    coord[1] = b.enu.n;

    double *p = coord;

    /* Clean up */
    proj_destroy (P);
    proj_context_destroy (C); /* may be omitted in the single threaded case */

    
    return p;
}

int main()
{

    string filename = "guerledan.pgm";
    ofstream ofst(filename); // ouverture en écriture
    
    // ..tests d’ouverture..
    if (!ofst.is_open())
    {
        cout << "Erreur d’ouverture de " << filename << endl;
    }
    
    //Initialisation du fichier avec le nombre magique P2 et les dimensions de l'image
    ofst << "P2" << endl << "1669 1572" << endl ;

    ofst.close();

    //---------------------------------------------------------------------------------------------------------------


    // Initialisation des variables 

    //Pour l'enveloppe rectangulaire
    int compteur_points = 1; //Compteur du nombre de points du fichier

    double min_n = 100000000;
    double min_e = 100000000;
    double max_n = -100000;
    double max_e = -100000;

    double etendue_x;
    double etendue_y;
    
    //Pour préparer l'échelle des couleurs ou des gris
    double min_alt = 1000000;
    double max_alt = -1000000;

    double etendue_alt;

    //Pour la projection
    double l93_e = 0.0;
    double l93_n = 0.0;
    // double* pt_coord;
    double coord[2];

    //---------------------------------------------------------------------------------------------------------------


    // Initialisations pour préparer la projection 
    PJ_CONTEXT *C;
    PJ *P;
    PJ* P_for_GIS;
    PJ_COORD a, b;

    C = proj_context_create();
    P = proj_create_crs_to_crs (C,"EPSG:4326", /* WGS84 */ "EPSG:2154", /* Lambert93 */NULL);


    /* This will ensure that the order of coordinates for the input CRS */
    /* will be longitude, latitude, whereas EPSG:4326 mandates latitude, */
    /* longitude */
    P_for_GIS = proj_normalize_for_visualization(C, P);
    proj_destroy(P);
    P = P_for_GIS;

    //---------------------------------------------------------------------------------------------------------------


    // ouverture du fichier Guerledan_Feb19_50cm_wgs84.txt pour lecture 

    ifstream f("/home/anton1/Documents/TD C++/projet_MNT/Guerledan_Feb19_50cm_wgs84.txt");

    if(!f.is_open())
        {
            cout << "Impossible d'ouvrir le fichier en lecture" << endl;
        }
        
    else // Section de la projection
        {
            string ligne; 

            // Initialisation des 3 variables de localisation
            double lat;
            double longi;
            double alt;

            

            while (getline(f, ligne))
            {
                // On attribue les valeurs lues aux variables associées
                f >> lat >> longi >> alt ;


                //pt_coord = projection(longi,lat,0.0,0.0);

                //-----------------------------------------------------------

                // Projection et récupération des nouvelles valeurs x et y 
                a = proj_coord (longi, lat,0.0,0.0);
                /* transform to Lamber93 */
                b = proj_trans (P, PJ_FWD, a);
                coord[0] = b.enu.e;
                coord[1] = b.enu.n;


                // l93_n = pt_coord[1];
                // l93_e = pt_coord[0];

                // Nouvelles coordonées
                l93_n = coord[1];
                l93_e = coord[0];

                // Recherches des valeurs minimales et maximales pour trouver l'enveloppe rectangulaire
                if (l93_n <= min_n)
                {
                    min_n = l93_n;
                }

                if (l93_e <= min_e)
                {
                    min_e = l93_e;
                }

                if (l93_n >= max_n)
                {
                    max_n = l93_n;
                }

                if (l93_e >= max_e)
                {
                    max_e = l93_e;
                }

                // Recherches des valeurs minimales et maximales pour trouver l'étendue de l'altitude

                if (alt <= min_alt)
                {
                    min_alt = alt;
                }

                if (alt >= max_alt)
                {
                    max_alt = alt;
                }

                compteur_points++;
                // Affichage des coordonées et de l'altitude si besoin
                //cout << "y, northing : " << l93_n << " | "<< "x, easting : " << l93_e << " | "<< "altitude : "<< alt << '\n'<< endl ;
            }
            
    
        }

    cout << "\n" << endl;
    cout << "x, easting min|max : " << min_e << " | " << max_e << "   " << "n, northing min|max : "<< min_n << " | " << max_n << endl;
    cout << "z altitude min|max : " << min_alt << " | " << max_alt<< endl;

    etendue_x = max_e - min_e ;
    etendue_y = max_n - min_n ;
    etendue_alt = abs(min_alt) - abs(max_alt) ;

    cout << "\n" << endl;
    cout << "largeur de l'enveloppe : " <<  etendue_y << " | " << "longueur de l'enveloppe : " <<  etendue_x << endl;
    cout << "étendue de l'altitude : " <<  etendue_alt << endl;

    cout << "\n" << endl;
    cout << "nombre de points : " << compteur_points <<  endl;


    /* Clean up */
    proj_destroy (P);
    proj_context_destroy (C); /* may be omitted in the single threaded case */

        return 0;
}