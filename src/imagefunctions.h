#include <QImage>
#include "math.h"

// taken from an old version of KDE Gwenview (GPL)

template<class T> inline const T& kClamp( const T& x, const T& low, const T& high )
inline int changeBrightness( int value, int brightness )
inline int changeContrast( int value, int contrast )
inline int changeGamma( int value, int gamma )
inline int changeUsingTable( int value, const int table[] )
template< int operation( int, int ) > static QImage changeImage( const QImage& image, int value )
QImage changeBrightness( const QImage& image, int brightness )
QImage changeContrast( const QImage& image, int contrast )
QImage changeGamma( const QImage& image, int gamma )
