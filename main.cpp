//#include "friendlist.h"
#include "ckernel.h"
#include <QApplication>

using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //FriendList w;
   // w.show();
    CKernel kernel;
    return a.exec();
}
