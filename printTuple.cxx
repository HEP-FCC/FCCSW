//
//  printTuple.cxx
//  
//
//  Created by Julia Hrdinka on 19/03/15.
//
//

#include <stdio.h>
/*

void printTuple (int i=0){
    cout << "Test: " << i << std::endl;

}
*/
void printTuple (const char* file, const char* draw, int color = 0, const char* drawcase = "", const char* drawopt = "" ){
    
    TNtuple *ntuple = new TNtuple("name","title", "x:y:z");
    ntuple->ReadFile(file);
    ntuple->SetMarkerColor(color);
    ntuple->Draw(draw, drawcase, drawopt);
}