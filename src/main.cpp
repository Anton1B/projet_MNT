#include <stdio.h>
#include <proj.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

//Guerledan_Feb19_50cm_wgs84.txt

// double* projection(double longi,double lat,double z1,double z2) 
// {
//     PJ_CONTEXT *C;
//     PJ *P;
//     PJ* P_for_GIS;
//     PJ_COORD a, b;

//     double *err;
//     double val_err = 1.0;
//     err = &val_err;

//     C = proj_context_create();

//     P = proj_create_crs_to_crs (C,
//                                 "EPSG:4326", /* WGS84 */
//                                 "EPSG:2154", /* Lambert93 */
//                                 NULL);

//     if (0==P) {
//         fprintf(stderr, "Oops\n");
//         return err;
//     }

//     /* This will ensure that the order of coordinates for the input CRS */
//     /* will be longitude, latitude, whereas EPSG:4326 mandates latitude, */
//     /* longitude */
//     P_for_GIS = proj_normalize_for_visualization(C, P);
//     if( 0 == P_for_GIS )  {
//         fprintf(stderr, "Oops\n");
//         return err;
//     }
//     proj_destroy(P);
//     P = P_for_GIS;

//     /* a coordinate union representing Copenhagen: 55d N, 12d E    */
//     /* Given that we have used proj_normalize_for_visualization(), the order of
//     /* coordinates is longitude, latitude, and values are expressed in degrees. */
//     //a = proj_coord (12, 55, 0, 0);
//     a = proj_coord (longi, lat, z1, z2);

//     /* transform to Lamber93 */
//     b = proj_trans (P, PJ_FWD, a);
//     //printf ("easting: %.3f, northing: %.3f\n", b.enu.e, b.enu.n);
    
//     double coord[2];
//     coord[0] = b.enu.e;
//     coord[1] = b.enu.n;

//     double *p = coord;

//     /* Clean up */
//     proj_destroy (P);
//     proj_context_destroy (C); /* may be omitted in the single threaded case */

    
//     return p;
// }



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
    ofst << "P2" << endl << "1706 1607" << endl << "1000" << endl ; // "1669 1572" "1706 1607"


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

    f.close();

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


    


    //----------------------------------------------------------------------------------------------------

   


    ifstream f_2("/home/anton1/Documents/TD C++/projet_MNT/Guerledan_Feb19_50cm_wgs84.txt");

    if(!f_2.is_open())
        {
            cout << "Impossible d'ouvrir le fichier en lecture" << endl;
        }
        
    else // Section de la projection
        {
            string ligne; 

            // Initialisation des 3 variables de localisation
            double lat_2;
            double longi_2;
            double alt_2;

            // Initialisation d'une liste représentant une ligne de pixels noirs

            int **set_point = new int*[1607];
            for(int i = 0; i < 1706; ++i)
            {
                set_point[i] = new int[1706];
            }
            
            for(int i = 0; i < 1607; ++i)
            {
                for(int j= 0; j < 1706; ++j)
                {
                    set_point[i][j] = 0;
                    //cout << "ok" << endl;
                }
            }
            //float set_point[1706] = {0} ;
            int compt = 1 ;

            int c = 0;
            int k = 0;

            // Variables pour la conversion de l'altitude en gris et le placement des points
            int save_set_e = 0;
            int set_e  = 0;
            int set_n = 0;
            float conv_alt_gris = 0;


            int diff; //Différence de position en x entre 2 valeurs successives

            int nb_cara = 0;

            while (getline(f_2, ligne))
            {
                // On attribue les valeurs lues aux variables associées
                f_2 >> lat_2 >> longi_2 >> alt_2 ;  


                 // Projection et récupération des nouvelles valeurs x et y 
                a = proj_coord (longi_2, lat_2,0.0,0.0);
                /* transform to Lamber93 */
                b = proj_trans (P, PJ_FWD, a);
                coord[0] = b.enu.e;
                coord[1] = b.enu.n;

                // Nouvelles coordonées
                l93_n = coord[1];
                l93_e = coord[0];

                // Placement en x
                l93_e -= min_e;
                set_e = int(l93_e);


                // Placement en y
                l93_n -= min_n;
                set_n = int(l93_n);
                //cout << set_n << endl;

                // Conversion de l'altitude en gris
                alt_2 = alt_2*-1;
                alt_2 += max_alt;

                conv_alt_gris = alt_2*(1000/etendue_alt); 


                
                // Si une valeur est en dehors de la ligne/colonne de pixels
                if (set_e >= 1706)
                {
                    set_e = 1705; //Le dernier pixel correspond à cette valeur
                }

                if (set_n >= 1607)
                {
                    set_n = 1606; //Le dernier pixel correspond à cette valeur
                }


                //Si il n'y a encore 2 valeurs successsives (premier tour de boucle)
                // if (compt == 1)
                // {
                //     save_set_e = set_e;
                // }

                // //On compare les valeurs successives
                // diff = save_set_e - set_e;
                // //cout << diff << endl;

                // //Si les deux valeurs successives en x sont espacées d'un certain seuil, la nouvelle valeur appartient à la ligne de pixels suivante
                // if (diff > 0)
                // {


                    // On écrit la ligne de pixel dans le fichier .pgm
                    // for (k=0; k<1706; k++)
                    // {
                    //     ofst << set_point[k] <<" ";


                    //     // nb_cara++;

                    //     // if (nb_cara%10==0)
                    //     // {
                    //     //     ofst <<"\n";
                    //     // }
                    // }

                    // //On remet la ligne de pixel à 0 pour avoir des pixels noirs
                    // for (c=0; c<1706; c++)
                    // {
                    //     set_point[c] = 0;
                    // }
            
                // }
                
                // save_set_e = set_e;

                // set_point[set_e] = conv_alt_gris;

                // compt++;

                
                set_point[set_n][set_e] = conv_alt_gris;
                    
                
            }
    
            


            for(int i = 1606; i > 0; i=i-1)
            {
                for(int j= 0; j < 1706 ; j++)
                {
                    ofst << set_point[i][j] <<" "; 
                }
            }
            
        }
    f_2.close();
    ofst.close();

    /* Clean up */
    proj_destroy (P);
    proj_context_destroy (C); /* may be omitted in the single threaded case */


    return 0;
}