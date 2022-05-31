#include<iostream>
#include<math.h>

void CreateCircle(float verts[],int indicies[], int resolution, float radius){
    float ang = 0;
    float incr = (2*M_PI)/resolution;
    verts[0] = verts[1] = verts [2] = 0.0f;
    for (int i=1;i<resolution+1;i++){
        verts[3*i] = radius*cosf(ang); if (abs(verts[3*i]) < 0.00001f) verts[3*i] = 0.0f; //rounding errors
        verts[3*i+1] = radius*sinf(ang); if (abs(verts[3*i+1]) < 0.00001f) verts[3*i+1] = 0.0f;
        verts[3*i+2] = 0.0f;
        ang+=incr;
    }
    for(int i=0;i<resolution-1;i++){
        indicies[3*i] = 0;
        indicies[3*i+1] = i+1;
        indicies[3*i+2] = i+2;
    }

    indicies[3*(resolution-1)] = 0;
    indicies[3*(resolution-1)+1] = resolution;
    indicies[3*(resolution-1)+2] = 1;
}

int main(){
    float verts[99];
    int indices[99];

    int resolution; std::cin>>resolution;
    CreateCircle(verts,indices,resolution,0.5f);
    for(int i=0;i<resolution+1;i++){
        std::cout<<verts[3*i]<<"f,"<<verts[3*i+1]<<"f,0.0f, //"<<i<<'\n';
    }
    for(int i=0;i<resolution;i++){
        std::cout<<indices[3*i]<<","<<indices[3*i+1]<<","<<indices[3*i+2]<<'\n';
    }
    return 0;
}
