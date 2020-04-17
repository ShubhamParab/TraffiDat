#include "model.h"

using namespace TraffiDat;

GraphColouring::GraphColouring(QList<road> upRds, QList<qint64> upAngles, QList<road> downRds, QList<qint64> downAngles, QMap<qint64, float *> probabs)
{
    for(int i = 0; i < upRds.size(); i++){
        for(int j = 0; j < downRds.size(); j++){
            routes.append(QPair<qint64, qint64>(upRds.at(i).road_id, downRds.at(j).road_id));
            angles.append(QPair<qint64, qint64>(upAngles.at(i), downAngles.at(j)));
            qDebug() << "ROUTE: " << upRds.at(i).road_id << ", " << downRds.at(j).road_id << " WITH ANGLES: " << upAngles.at(i) << ", " << downAngles.at(j);
        }
    }
    for(int i = 0; i < routes.size(); i++){
        QVector<qint64> row(routes.size());
        for(int j = 0; j < routes.size(); j++){
            qDebug() << "UP1: " << routes.at(i).first << " DOWN1: " << routes.at(i).second;
            qDebug() << "UP2: " << routes.at(j).first << " DOWN2: " << routes.at(j).second;
            if(i == j){
                qDebug() << "At: " << i << ", " << j << " setting 0";
                row.insert(j, 0);
                continue;
            }
            if(is_compatible(angles.at(i).first, angles.at(i).second, angles.at(j).first, angles.at(j).second, *probabs.value(routes.at(i).first), *probabs.value(routes.at(j).first))){
                qDebug() << "COMPATIBLE: At: " << i << ", " << j << " setting 1";
                row.insert(j, 1);
            }else{
                qDebug() << "NOT COMPATIBLE: At: " << i << ", " << j << " setting 0";
                row.insert(j, 0);
            }
            qDebug() << "___________________________________";
        }
        graph.insert(i, row);
    }

//    printf("PRINTING GRAPH");
//    for(int i = 0; i < graph.size(); i++){
//        QVector<qint64> row = graph.at(i);
//        for(int j = 0; j < row.size(); j++){
//            qDebug() << row.at(j) << "\t";
//        }
//        qDebug() << "\n";
//    }

    V = routes.size();
    color = new int[V];
}

void GraphColouring::printSolution(int color[])
{
    printf("Solution Exists:"
                " Following are the assigned colors \n");
        for (int i = 0; i < V; i++)
          printf(" %d ", color[i]);
        printf("\n");
}

bool GraphColouring::isSafe(int v, int color[], int c)
{
    for (int i = 0; i < V; i++)
            if (graph[v][i] && c == color[i])
                return false;
    return true;
}

bool GraphColouring::graphColoringUtil(int m, int color[], int v)
{
    /* base case: If all vertices are assigned a color then
           return true */
        if (v == V)
            return true;

        /* Consider this vertex v and try different colors */
        for (int c = 1; c <= m; c++)
        {
            /* Check if assignment of color c to v is fine*/
            if (isSafe(v, color, c))
            {
               color[v] = c;

               /* recur to assign colors to rest of the vertices */
               if (graphColoringUtil (m, color, v+1) == true)
                 return true;

                /* If assigning color c doesn't lead to a solution
                   then remove it */
               color[v] = 0;
            }
        }

        /* If no color can be assigned to this vertex then return false */
        return false;
}

bool GraphColouring::graphColoring(int m)
{
    // Initialize all color values as 0. This initialization is needed
        // correct functioning of isSafe()
        color = new int[V];
        for (int i = 0; i < V; i++)
           color[i] = 0;

        // Call graphColoringUtil() for vertex 0
        if (graphColoringUtil(m, color, 0) == false)
        {
          printf("Solution does not exist");
          return false;
        }

        // Print the solution
        printSolution(color);
        return true;
}

double getMin(QVector<double> ls)
{
    double out = 0;
    for(double i : ls){
        if(i < out)
            out = i;
    }
    return out;
}

double getMax(QVector<double> ls)
{
    double out = 0;
    for(double i : ls){
        if(i > out)
            out = i;
    }
    return out;
}
