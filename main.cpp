#include <iostream>

#include <fstream>
#include <iostream>
#include <string.h> //contains string functions
#include <string>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_LENGTH 1024 * 4

using namespace std;

// assume the CMU cgspeed category. After the code runs, the directory will contain one bvh file for
// each group

string base_dir = "/home/samuel/Downloads/";
int no_dir = 1;

//this controls the frame rate of the merged bvh file
float default_frame_rate = 0.00833333;

int main(int argc, char *argv[]) {
    char line[BUFFER_LENGTH];
    char *token;

    char separater[] = " \r:,\t";

    for (int i = 1; i <= no_dir; i++) { // directory

        long int totalFrame =0;

        // buffers for saving the file

        // construct the local base directory

        string sub_dir;

        if (i < 10)
            sub_dir = "0" + to_string(i) + "/" + "0" + to_string(i) + "_";
        else
            sub_dir = to_string(i) + to_string(i) + "_";
        // construct the file

        // files for the merged bvh
        string mergedFile = base_dir + sub_dir + "merged.bvh";
        string appendFile = base_dir + sub_dir + "append";

        ofstream outMerged(mergedFile);

        ofstream outAppendBuffer(appendFile); // to concatenate motion

        int cFile = 1;

        while (true) {
            string filePath = base_dir;
            filePath += sub_dir;

            if (cFile < 10)
                filePath += "0";
            filePath += to_string(cFile);
            filePath += ".bvh";
            ifstream file(filePath);

            if (!file.is_open())
                break;

            bool startAppend = false;
            while (!file.eof()) {

                file.getline(line, BUFFER_LENGTH);

                if (startAppend) {

                    //send the read line into the append file
                    outAppendBuffer << line << endl;
                    totalFrame++;//increment total frame count

                    continue;
                }

                string lineString = string(line);
                token = strtok(line, separater);

                if (strcmp("Frames", token) == 0) {

                    startAppend = true;
                    //discard the next line aswell
                    file.getline(line, BUFFER_LENGTH);
                    continue;
                }

                //finally write the line into file if its the first file
                if(cFile == 1){
                    outMerged << lineString << endl;
                }

                // start the append here
            }

            cFile++;
        }
        //add the necessary info
        outMerged<< "Frames: " << totalFrame << endl;
        outMerged<< "Frame Time: " << default_frame_rate << endl;

        outMerged.close();
        outAppendBuffer.close();

         //now open the buffer file and dump the motion info with raw read/write
         FILE* outMergedFile = fopen(mergedFile.c_str(), "ab");
         FILE* outAppendBufferFile = fopen(appendFile.c_str(), "rb");

         fseek(outAppendBufferFile, 0, SEEK_END);

         long fsize = ftell(outAppendBufferFile);

         rewind(outAppendBufferFile);

         char *data = (char*)malloc( fsize + 2);

         fread(data, fsize, 1, outAppendBufferFile);

         fclose(outAppendBufferFile);

         data[fsize] = 0;

         data[fsize+1] = '\n';

         //write this data
        // fseek(outMergedFile, 0, SEEK_END);

         fwrite(data , sizeof(char), fsize , outMergedFile);

         fclose(outMergedFile);

         free(data);

        // delete the append file when done
        system("rm -rf appendFile");
    }
}
