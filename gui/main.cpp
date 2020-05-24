#include <QApplication>

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    a.setApplicationName("otpgen");
    a.setApplicationDisplayName("OTPGen");
    a.setWindowIcon(QIcon(":/app-icon.svgz"));

    return 0;
}
