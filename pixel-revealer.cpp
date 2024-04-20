#include <iostream>
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include <ctime>

using namespace std;
using namespace cv;

int main() {
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = TEXT("Image Files\0*.png;*.jpg;*.jpeg;*.bmp\0");
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_DONTADDTORECENT;

    if (GetOpenFileName(&ofn)) {
        string imagePath = ofn.lpstrFile;

        int pixelCount;
        cout << "Enter the number of pixels to reveal: ";
        cin >> pixelCount;

        Mat image = imread(imagePath);

        if (image.empty()) {
            cout << "Error: Unable to load image." << endl;
            return -1;
        }

        namedWindow("Image", WINDOW_NORMAL);
        resizeWindow("Image", image.cols, image.rows);

        srand(static_cast<unsigned int>(time(nullptr)));

        for (int count = 0; count < pixelCount; ++count) {
            int randomRow = rand() % image.rows;
            int randomCol = rand() % image.cols;

            Vec3b& pixel = image.at<Vec3b>(randomRow, randomCol);
            pixel[0] = 0;
            pixel[1] = 0;
            pixel[2] = 0;

            imshow("Image", image);
            waitKey(1);
        }

        waitKey(0);
        destroyAllWindows();
    }

    return 0;
}
