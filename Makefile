objects := Cursor.o Image.o Engine.o Object.o SceneLayer.o Text.o ballGun.o globaldefs.o hud.o main.o myEngine.o myEngine_new.o myEngine_retro.o
libs := -lhge_helpers -lhge -lSDLmain -lSDL -lvorbis -lvorbisfile -lopenal -lalut -ljpeg -lpng -logg -ltinyxml2 -lttvfs -lBox2D

all: Heartlight++

Heartlight++: $(objects)
	g++ -o $@ $^ $(libs)

%.o: %.cpp
	g++ -c -MMD -o $@ $<

-include $(objects:.o=.d)

clean:
	rm -f *.o *.d Heartlight++
