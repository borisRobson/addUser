#include <stdio.h>
#include "headers/captureimage.h"

using namespace std;

int main(int argv, char **argc)
{
    Q_UNUSED(argv);
    Q_UNUSED(argc);
    captureimage* imcap;

    imcap = new captureimage();

    imcap->showimage();

    cv::destroyAllWindows();

    return 0;
}
