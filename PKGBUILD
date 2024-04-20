# Maintainer: Wellington <wellingtonwallace@gmail.com>

pkgname=fastgame-git
pkgver=0.3.0.r37.ga5943b6
pkgrel=1
pkgdesc='Optimize system performance for games'
arch=(x86_64 i686)
url='https://github.com/wwmm/fastgame'
license=('GPL3')
depends=('boost-libs' 'kirigami' 'kirigami-addons' 'qqc2-desktop-style' 'libxnvctrl')
makedepends=('boost' 'cmake' 'extra-cmake-modules' 'git')
source=("git+https://github.com/wwmm/fastgame.git#branch=fgqt")
conflicts=(fastgame)
provides=(fastgame)
sha512sums=('SKIP')

pkgver() {
  cd fastgame
  git describe --long | sed 's/^v//;s/\([^-]*-g\)/r\1/;s/-/./g'
}

build() {
  mkdir build
  cd build
  cmake ..
  cmake --build .
}

package() {
  DESTDIR="${pkgdir}" ninja install -C build
}
