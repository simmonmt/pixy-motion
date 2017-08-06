OPENCV_CFLAGS=$$(/usr/local/bin/pkg-config /usr/local/opt/opencv3/lib/pkgconfig/opencv.pc --cflags)
OPENCV_LDFLAGS=$$(/usr/local/bin/pkg-config /usr/local/opt/opencv3/lib/pkgconfig/opencv.pc --libs)

LIBS= \
	/Users/simmonmt/src/pixy/lib/libpixyusb.a \
	-lusb-1.0 \
	-lboost_thread-mt -lboost_system -lboost_chrono

CFLAGS= \
	$(OPENCV_CFLAGS) \
	-I/Users/simmonmt/src/pixy/include \
	-I/usr/local/include/libusb-1.0

LDFLAGS= \
	$(OPENCV_LDFLAGS) \
	-L/usr/local/lib \
	$(LIBS)

capture: capture.cc
	c++ $(CFLAGS) $(LDFLAGS) -o $@ $<

#	cc -std=c++11 -arch x86_64 -I/Users/simmonmt/src/opencv/install/include -L/Users/simmonmt/src/opencv/install/lib -L/usr/local/lib -I/usr/local/include/libusb-1.0  -I../include -o capture capture.cc ../lib/libpixyusb.a  -lusb-1.0  -lstdc++ -lboost_thread-mt -lboost_system -lboost_chrono  -lopencv_core -lopencv_highgui -lopencv_imgproc -Xlinker -rpath -Xlinker /Users/simmonmt/src/opencv/install/lib
