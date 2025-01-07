#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <stack>
#include "channel_name.h"
#include "ftra.h"
using namespace std;

void Graph::addEdge(int sv,int dv){
    struct Edge temp_edge;
    temp_edge.sv = sv;
    temp_edge.dv = dv;
    temp_edge.next_edge = this->VertexSet[sv].first_edge;
    this->EdgeSet.push_back(temp_edge);
    this->VertexSet[sv].first_edge = this->EdgeSet.size()-1;
    // cout << "added an edge" << endl;
}

void Graph::rmvEdge(int sv,int dv){
    int last_eid;
    int now_eid;
    now_eid = this->VertexSet[sv].first_edge;
    if(this->EdgeSet[now_eid].dv==dv){
        this->VertexSet[sv].first_edge = this->EdgeSet[now_eid].next_edge;
        // cout << "removed edge (" << sv << "," << dv << ")" << endl;
        return;
    }
    while(this->EdgeSet[now_eid].dv!=dv){
        if(this->EdgeSet[now_eid].next_edge==-1){
            // cout << "rmvEdge failed:cannot find object edge: (" << sv << "," << dv << ")" << endl;
            return;
        }
        last_eid = now_eid;
        now_eid = this->EdgeSet[last_eid].next_edge;
    }
    this->EdgeSet[last_eid].next_edge=this->EdgeSet[now_eid].next_edge;
    // cout << "removed edge (" << sv << "," << dv << ")" << endl;
}

Edge* Graph::getEdge(int sv,int dv){
    int eid;
    eid = this->VertexSet[sv].first_edge;
    while(eid!=-1){
        if(this->EdgeSet[eid].dv==dv) return &this->EdgeSet[eid];
        eid = this->EdgeSet[eid].next_edge;
    }
    cout << "getEdge failed:cannot find object edge: (" << sv << "," << dv << ")" << endl;
    return &this->EdgeSet[0];
}

void Graph::addEdgeweight(int weight, Edge* edge){
    edge->weight += weight;
}

void Graph::makeweightzero(){
    for(unsigned int i=0;i<this->EdgeSet.size();i++){
        this->EdgeSet[i].weight = 0;
    }
}

void Graph::setEdgeweight(int weight, Edge* edge){
    edge->weight = weight;
}

void Graph::addEdgesFrom(vector<int> &rt){
    for(unsigned int i=0;i<(rt.size()>>1);i++){
        this->addEdge(rt[i<<1],rt[(i<<1)+1]);
    }
}

void Graph::rmvEdgesFrom(vector<int> &pt){
    for(unsigned int i=0;i<(pt.size()>>1);i++){
        this->rmvEdge(pt[i<<1],pt[(i<<1)+1]);
    }
}

void Graph::printEdges(){
    int eid;
    cout << "printEdges" << endl;
    for(int i=0;i<this->vnum;i++){
        eid = this->VertexSet[i].first_edge;
        if(eid==-1) continue;
        while(this->EdgeSet[eid].next_edge!=-1){
            cout << '(' << i << ',' << EdgeSet[eid].dv << ')' << endl;
            eid = EdgeSet[eid].next_edge;
        }
        cout << '(' << i << ',' << EdgeSet[eid].dv << ')' << endl;
    }
}

bool Graph::hasLoop(){
    vector<bool> visited(this->vnum);
    for(int i=0;i<this->vnum;i++){
        if(this->VertexSet[i].first_edge==-1) continue;
        visited = vector<bool>(this->vnum,false);
        if(this->DFS(i,i,visited)){
            return true; // if find loop, stop searching
        }
    } 
    return false;
}

bool Graph::hasPath(int sv,int dv){
    vector<bool> visited(this->vnum);
    return this->DFS(dv,sv,visited);
}

bool Graph::hasPathwithroute(int sv,int dv, vector<int> &path){
    vector<bool> visited(this->vnum);
    return this->DFSwithroute(dv,sv,visited,path);
}

bool Graph::hasPathwithroute1(int sv,int dv, vector<int> &path){
    return this->BFSwithroute(dv,sv,path);
}

bool Graph::DFS(int tgt_vid,int now_vid,vector<bool> &visited){
    //this function searches a loop/path from a given vertex(now_id)
    //to find a loop, set tgt_vid as the source vertex
    //to find a path, set tgt_vid as the destination vertex
    struct Edge edge;
    int eid;
    visited[now_vid] = true;
    // cout << now_vid << endl;
    eid = this->VertexSet[now_vid].first_edge;
    while(eid!=-1){
        edge = this->EdgeSet[eid];
        if(edge.dv==tgt_vid){ 
            return true; // if find loop/path, stop searching
        }
        if(!visited[edge.dv]){ //not visited
            if(this->DFS(tgt_vid,edge.dv,visited)) 
                return 1; // if find loop/path, stop searching
        }
        eid = edge.next_edge;
    }
    return false;
}

bool Graph::DFSwithroute(int tgt_vid, int now_vid, vector<bool> &visited, vector<int> &path){
    // 该函数从当前顶点(now_vid)搜索到目标顶点(tgt_vid)的路径
    // 如果找到路径，则返回true，并将路径记录在path中
    struct Edge edge;
    int eid;
    visited[now_vid] = true;
    path.push_back(now_vid); // 将当前顶点加入路径
    eid = this->VertexSet[now_vid].first_edge;
    while(eid != -1){
        edge = this->EdgeSet[eid];
        if(edge.dv == tgt_vid){
            path.push_back(tgt_vid); // 找到目标顶点，加入路径
            return true; // 找到路径，结束搜索
        }
        if(!visited[edge.dv]){
            if(this->DFSwithroute(tgt_vid, edge.dv, visited, path)){
                return true; // 如果在递归调用中找到路径，继续返回
            }
        }
        eid = edge.next_edge;
    }
    path.pop_back(); // 回溯，移除当前顶点
    return false; // 未找到路径
}

bool Graph::BFSwithroute(int tgt_vid, int now_vid, vector<int> &path) {
    int tgtx=tgt_vid%4;
    int tgty=tgt_vid/4;
    int nowx=now_vid%4;
    int nowy=now_vid/4;
    int curid=now_vid*12+MS_LOCAL_I;
    int destid=tgt_vid*12+MS_LOCAL_O;
    path.push_back(curid); // 将当前顶点加入路径
    while ((nowy*4+nowx)*12+MS_LOCAL_O!=destid) {
        if(nowx!=tgtx){
            if(nowx>tgtx){
                path.push_back((nowy*4+nowx)*12+MS_LEFT_O); //向左
                nowx--; //向左
                path.push_back((nowy*4+nowx)*12+MS_RIGHT_I);
            } //向左
            else if(nowx<tgtx){
                path.push_back((nowy*4+nowx)*12+MS_RIGHT_O); //向右
                nowx++; //向右
                path.push_back((nowy*4+nowx)*12+MS_LEFT_I);
            } //向右
        }
        else{
            if(nowy>tgty){
                path.push_back((nowy*4+nowx)*12+MS_UP_O); 
                nowy--; 
                path.push_back((nowy*4+nowx)*12+MS_DOWN_I);
            } //向上
            else if(nowy<tgty){
                path.push_back((nowy*4+nowx)*12+MS_DOWN_O); 
                nowy++; 
                path.push_back((nowy*4+nowx)*12+MS_UP_I);
            } //向下
        }
    }
    path.push_back(destid); // 将目标顶点加入路径
    return true; // 队列为空，未找到路径
}

void Graph::searchOut(int now_vid,vector<int> &dist,int depth){
    int eid;
    struct Edge edge;
    eid = this->VertexSet[now_vid].first_edge;
    while(eid!=-1){
        edge = this->EdgeSet[eid];
        if(edge.dv==this->vnum-1){ //the abstract node
            dist.push_back(depth);
            return;
        }
        this->searchOut(edge.dv,dist,depth+1);
        eid = edge.next_edge;
    }
}

void Graph::searchIn(int now_vid,int dst_vid,vector<int> &dist,int depth){
    int eid;
    struct Edge edge;
    eid = this->VertexSet[now_vid].first_edge;
    while(eid!=-1){
        edge = this->EdgeSet[eid];
        if(edge.dv==this->vnum-1){ //the abstract node
            return;
        }
        if(edge.dv==dst_vid){
            dist.push_back(depth); //the destination node
            return;
        }
        this->searchIn(edge.dv,dst_vid,dist,depth+1);
        eid = edge.next_edge;
    }
}
