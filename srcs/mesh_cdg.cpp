#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "channel_name.h"
#include "ftra.h"
using namespace std;

//construction function
MeshCdg::MeshCdg(int w,int h){
    struct Vertex temp_vert;
    this->vnum = w*h*12+1;
    this->W = w;
    this->H = h;
    for(int i=0; i<w; i++){
        for(int j=0; j<h; j++){
            for(int k=0; k<12; k++){
                temp_vert.id = k + j*12 + i*h*12; 
                this->VertexSet.push_back(temp_vert); //original nodes
            }
        }
    }
    temp_vert.id = this->vnum-1;
    this->VertexSet.push_back(temp_vert); //abstract node
    this->setOriginEdges();
    // cout << "construction completed" << endl;
}

bool MeshCdg::hasGLoop(){
    vector<bool> visited(this->vnum);
    if(this->DFS(this->vnum-1,this->vnum-1,visited)) 
        return true; 
    return false;
}

bool MeshCdg::isConnected(){
    int base;
    for(int i=0;i<this->W*this->H;i++){
        base=12*i;
        if(!(this->hasPath(base+MS_LOCAL_I,this->vnum-1)))
            return false;
        else{
            if(!(this->hasPath(this->vnum-1,base+MS_LOCAL_O)))
                return false;
        }
    }
    return true;
}

bool MeshCdg::isAllConnected(string A){
    int base;
    vector<int> path;
    MeshCode code,code1,code2;
    if(A=="human"){
    for(int i=0;i<this->W*this->H;i++){
        base=12*i;
        if((this->hasPathwithroute(base+MS_LOCAL_I,this->vnum-1,path))==false)
            {return false;}
        path.clear();     
        if((this->hasPathwithroute(base+MS_LOCAL_I,this->vnum-1,path))==true){
            MeshCdg::decodeVid(code1,base+MS_LOCAL_I);
            MeshCdg::decodeVid(code2,this->vnum-1);
            cout << "path from " << "(" << code1.x << "," << code1.y << "," << code1.chan << ")" << " to " 
            <<"outside node"<< endl;
            for(unsigned int j=0;j<path.size();j++){
                if(path[j]==this->vnum-1) {cout << "outside node";}
                else{MeshCdg::decodeVid(code,path[j]);
                cout << "(" << code.x << "," << code.y << "," << code.chan << ")-->";}
            }
            cout << endl;
            path.clear();
        }
        if(!(this->hasPathwithroute(this->vnum-1,base+MS_LOCAL_O,path))){
                return false;}
        path.clear();   
        if((this->hasPathwithroute(this->vnum-1,base+MS_LOCAL_O,path))==true){
            MeshCdg::decodeVid(code1,this->vnum-1);
            MeshCdg::decodeVid(code2,base+MS_LOCAL_O);
            cout << "path from " << "outside node" << " to "
            << "(" << code2.x << "," << code2.y << "," << code2.chan << ")" << endl;
            for(unsigned int j=0;j<path.size();j++){
                if(path[j]==this->vnum-1) {cout << "outside node";}
                else{
                MeshCdg::decodeVid(code,path[j]);
                MeshCdg::decodeVid(code,path[j]);
                cout << "-->(" << code.x << "," << code.y << "," << code.chan << ")";}
            }
            cout << endl;
            path.clear();   
        } 
    }
    path.clear();
    for(int i=0;i<this->W*this->H;i++){
        for(int j=0;j<this->W*this->H;j++){
            int startbase = i*12;
            int endbase = j*12;
            MeshCode code1,code2;
            if(i==j) continue;
            if(!(this->hasPathwithroute(startbase+MS_LOCAL_I,endbase+MS_LOCAL_O,path)))
                {path.clear();   
                return false;}
            else{
                MeshCdg::decodeVid(code1,startbase+MS_LOCAL_I);
                MeshCdg::decodeVid(code2,endbase+MS_LOCAL_O);
                cout << "path from " << "(" << code1.x << "," << code1.y << "," << code1.chan << ")" << " to " 
                << "(" << code2.x << "," << code2.y << "," << code2.chan << ")" << endl;
            for(unsigned int k=0;k<path.size();k++){
                if(path[k]==startbase+MS_LOCAL_I) {cout <<"("<< code.x << "," << code.y << "," << code.chan << ")";}
                else{MeshCdg::decodeVid(code,path[k]);
                cout << "-->(" << code.x << "," << code.y << "," << code.chan << ")";}
            }
            cout << endl;
            path.clear();
            }
        }
    }
    }
    else{
        for(int i=0;i<this->W*this->H;i++){
        base=12*i;
        if((this->hasPathwithroute(base+MS_LOCAL_I,this->vnum-1,path))==false)
            {path.clear();
            return false;}
            path.clear();
        if((this->hasPathwithroute(base+MS_LOCAL_I,this->vnum-1,path))==true){
            cout << "path from " << base+MS_LOCAL_I << " to " << this->vnum-1 << endl;
            for(unsigned int j=0;j<path.size();j++){
                if(path[j]==this->vnum-1) {cout << path[j]<<" ";}
                else{cout << path[j] << " ";}
            }
            cout << endl;
            path.clear();
        }
        if(!(this->hasPathwithroute(this->vnum-1,base+MS_LOCAL_O,path))){
                path.clear();
                return false;}
        path.clear();
        if((this->hasPathwithroute(this->vnum-1,base+MS_LOCAL_O,path))==true){
           cout << "path from " << this->vnum-1 << " to " << base+MS_LOCAL_O << endl;
            for(unsigned int j=0;j<path.size();j++){
                if(path[j]==this->vnum-1) {cout << path[j]<<" ";}
                else{cout << path[j] << " ";}
            }
            cout << endl;
            path.clear();   
        } 
    }
    path.clear();
    for(int i=0;i<this->W*this->H;i++){
        for(int j=0;j<this->W*this->H;j++){
            int startbase = i*12;
            int endbase = j*12;
            MeshCode code1,code2;
            if(i==j) continue;
            if(!(this->hasPathwithroute(startbase+MS_LOCAL_I,endbase+MS_LOCAL_O,path)))
                {path.clear();
                return false;}
            else{
                cout << "path from " << startbase+MS_LOCAL_I << " to " << endbase+MS_LOCAL_O << endl;
            for(unsigned int k=0;k<path.size();k++){
                if(path[k]==startbase+MS_LOCAL_I) {cout<<path[k]<<" ";}
                else{cout << path[k] << " ";}
            }
            cout << endl;
            path.clear();
            }
        }
    }
    }
    
    return true;
}

int MeshCdg::getBoundDist(bool dir,int rid){
    vector<int> dist; //all possible distances
    int min_dist,vid;
    if(dir){
        // cout << "search out" << endl;
        vid=rid*12+MS_LOCAL_I;
        this->searchOut(vid,dist,0);
    }
    else{
        vid=rid*12+MS_LOCAL_O;
        // cout << "search in" << endl;
        // cout << vid << endl;
        this->searchIn(this->vnum-1,vid,dist,0);
        
    }
    for(unsigned int i=0;i<dist.size();i++){
        if(i==0) min_dist = dist[0];
        else{
            if(dist[i] < min_dist)
                min_dist = dist[i];
        }
    }
    if(dist.empty()) return -1;
    return min_dist+1;
}

void MeshCdg::setBoundRouters(vector<int> brl){    
    int base;
    this->brl = brl;
    for(unsigned int i=0;i<this->brl.size();i++){
        base=this->brl[i]*12;
        this->addEdge(base+MS_BOUND_I,base+MS_LEFT_O);
        this->addEdge(base+MS_BOUND_I,base+MS_RIGHT_O);
        this->addEdge(base+MS_BOUND_I,base+MS_UP_O);
        this->addEdge(base+MS_BOUND_I,base+MS_DOWN_O);
        this->addEdge(base+MS_BOUND_I,base+MS_LOCAL_O);
        this->addEdge(base+MS_LEFT_I,base+MS_BOUND_O);
        this->addEdge(base+MS_RIGHT_I,base+MS_BOUND_O);
        this->addEdge(base+MS_UP_I,base+MS_BOUND_O);
        this->addEdge(base+MS_DOWN_I,base+MS_BOUND_O);
        this->addEdge(base+MS_LOCAL_I,base+MS_BOUND_O);
        this->addEdge(base+MS_BOUND_O,this->vnum-1);
        this->addEdge(this->vnum-1,base+MS_BOUND_I);
    }
}

void MeshCdg::initBoundTurns(vector<int> &bt){
    int base;
    for(unsigned int i=0;i<this->brl.size();i++){
        base=this->brl[i]*12;
        if(this->brl[i] % this->W != 0){
            bt.push_back(base+MS_BOUND_I);bt.push_back(base+MS_LEFT_O);
            bt.push_back(base+MS_LEFT_I);bt.push_back(base+MS_BOUND_O);
        }
        if(this->brl[i] % this->W != this->W-1){
            bt.push_back(base+MS_BOUND_I);bt.push_back(base+MS_RIGHT_O);
            bt.push_back(base+MS_RIGHT_I);bt.push_back(base+MS_BOUND_O);
        }
        if(this->brl[i] / this->W !=0){
            bt.push_back(base+MS_BOUND_I);bt.push_back(base+MS_UP_O);
            bt.push_back(base+MS_UP_I);bt.push_back(base+MS_BOUND_O);
        }
        if(this->brl[i] / this->W != this->H-1){
            bt.push_back(base+MS_BOUND_I);bt.push_back(base+MS_DOWN_O);
            bt.push_back(base+MS_DOWN_I);bt.push_back(base+MS_BOUND_O);
        }
    }
}

int MeshCdg::getAvgBoundDst(){
    int dst;
    dst=0;
    for(int i=0;i<this->W*this->H;i++){
        dst += this->getBoundDist(1,i);
        dst += this->getBoundDist(0,i);
    }
    return dst;
}

int MeshCdg::getAvgBoundRch(){
    int base_bound,base_local,rch,i_rch,o_rch;
    rch=0;
    for(unsigned int i=0;i<this->brl.size();i++){
        base_bound = this->brl[i]*12;
        i_rch=0;o_rch=0;
        for(int j=0;j<this->W*this->H;j++){
            base_local = j*12;
            if(this->hasPath(base_bound+MS_BOUND_I,base_local+MS_LOCAL_O)) i_rch++;
            if(this->hasPath(base_local+MS_LOCAL_I,base_bound+MS_BOUND_O)) o_rch++;
        }
        rch += i_rch + o_rch;
    }
    return rch;
}

void MeshCdg::rmvBoundLinks(){
    int base;
    for(unsigned int i=0;i<this->brl.size();i++){
        base = this->brl[i]*12;
        this->rmvEdge(base+MS_BOUND_O,this->vnum-1);
        this->rmvEdge(this->vnum-1,base+MS_BOUND_I);
    }
}

void MeshCdg::repBoundLinks(){
    int base;
    for(unsigned int i=0;i<this->brl.size();i++){
        base = this->brl[i]*12;
        this->addEdge(base+MS_BOUND_O,this->vnum-1);
        this->addEdge(this->vnum-1,base+MS_BOUND_I);
    }
}


void MeshCdg::setOriginEdges(){
    int base = 0;
    int neigh = 0;
    for(int i=0;i<this->W*this->H;i++){
        base = i*12;
        this->addEdge(base+MS_LEFT_I,base+MS_RIGHT_O);
        this->addEdge(base+MS_RIGHT_I,base+MS_LEFT_O);
        this->addEdge(base+MS_UP_I,base+MS_DOWN_O);
        this->addEdge(base+MS_DOWN_I,base+MS_UP_O);

        // valid turns
        this->addEdge(base+MS_LEFT_I,base+MS_UP_O);//
        this->addEdge(base+MS_LEFT_I,base+MS_DOWN_O);//
        this->addEdge(base+MS_RIGHT_I,base+MS_UP_O);//
        this->addEdge(base+MS_RIGHT_I,base+MS_DOWN_O);//

        // invalid turns
        // this->addEdge(base+MS_UP_I,base+MS_LEFT_O);//
        // this->addEdge(base+MS_UP_I,base+MS_RIGHT_O);//
        // this->addEdge(base+MS_DOWN_I,base+MS_LEFT_O);//
        // this->addEdge(base+MS_DOWN_I,base+MS_RIGHT_O);//

        this->addEdge(base+MS_LOCAL_I,base+MS_LEFT_O);
        this->addEdge(base+MS_LOCAL_I,base+MS_RIGHT_O);
        this->addEdge(base+MS_LOCAL_I,base+MS_UP_O);
        this->addEdge(base+MS_LOCAL_I,base+MS_DOWN_O);
        this->addEdge(base+MS_LEFT_I,base+MS_LOCAL_O);
        this->addEdge(base+MS_RIGHT_I,base+MS_LOCAL_O);
        this->addEdge(base+MS_UP_I,base+MS_LOCAL_O);
        this->addEdge(base+MS_DOWN_I,base+MS_LOCAL_O);
    }
    for(int i=0;i<this->W-1;i++){
        for(int j=0;j<this->H;j++){
            base = (j * this->W + i) * 12;
            neigh = (j * this->W + i + 1) * 12;
            this->addEdge(base+MS_RIGHT_O,neigh+MS_LEFT_I);
            this->addEdge(neigh+MS_LEFT_O,base+MS_RIGHT_I);
        }
    }
    for(int i=0;i<this->W;i++){
        for(int j=0;j<this->H-1;j++){
            base = (j * this->W + i) * 12;
            neigh = ((j+1) * this->W + i) * 12;
            this->addEdge(base+MS_DOWN_O,neigh+MS_UP_I);
            this->addEdge(neigh+MS_UP_O,base+MS_DOWN_I);
        }
    }
}

int MeshCdg::routeAtSrc(int srid,int drid){
    int srxid,sryid,drxid,dryid;
    if(srid == drid){
        cout << "error at routeAtSrc:same srid and drid" << endl;
        return -1;
    }
    srxid = srid % this->H;
    sryid = srid / this->H;
    drxid = drid % this->H;
    dryid = drid / this->H;
    if(srxid != drxid){
        if(srxid > drxid) return MS_LEFT_O;
        else return MS_RIGHT_O;
    }
    else if(sryid > dryid) return MS_UP_O;
    else return MS_DOWN_O;
}

int MeshCdg::routeAtDst(int srid,int drid){
    int srxid,sryid,drxid,dryid;
    if(srid == drid){
        cout << "error at routeAtSrc:same srid and drid" << endl;
        return -1;
    }
    srxid = srid % this->H;
    sryid = srid / this->H;
    drxid = drid % this->H;
    dryid = drid / this->H;
    if(sryid != dryid){
        if(sryid > dryid) return MS_DOWN_I;
        else return MS_UP_I;
    }
    else if(srxid > drxid) return MS_RIGHT_I;
    else return MS_LEFT_I;
}

//get vertex id from a router id and a bias channel id
//every topology has its own mapping function
int MeshCdg::encodeVid(int rid,int cid){
    return 12*rid+cid;
}

//code is a 3-element 
void MeshCdg::decodeVid(struct MeshCode &code,int vid){
    int chan_id = vid % 12;
    string chan;
    code.x = (vid/12) % this->H;
    code.y = (vid/12) / this->H;
    switch(chan_id){
        case 0 : chan = "left_i";   break;
        case 1 : chan = "left_o";   break;
        case 2 : chan = "down_o";   break;
        case 3 : chan = "down_i";   break;
        case 4 : chan = "right_i";  break;
        case 5 : chan = "right_o";  break;
        case 6 : chan = "up_o";     break;
        case 7 : chan = "up_i";     break;
        case 8 : chan = "local_i";  break;
        case 9 : chan = "local_o";  break;
        case 10: chan = "bound_i";  break;
        case 11: chan = "bound_o";  break;
        default: break;
    }
    code.chan = chan;
}