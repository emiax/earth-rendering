#include "sgct.h"
#include <ghoul/ghoul.h>
#include <ghoul/opengl/programobject.h>
#include <ghoul/io/texture/texturereader.h>
#include <ghoul/io/texture/texturereaderdevil.h>
#include <ghoul/systemcapabilities/systemcapabilities.h>
#include <ghoul/systemcapabilities/openglcapabilitiescomponent.h>
#include <renderer.h>
#include <earth.h>
#include <camera.h>
#include <sky.h>
#include <sun.h>
#include <cmath>
#include <binarystream.h>
#include <camerapath.h>

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif


sgct::Engine * gEngine;
earthrendering::Renderer * renderer;
earthrendering::Camera * camera;
earthrendering::Earth * earth;
earthrendering::Sun * sun;
earthrendering::Sky * sky;
earthrendering::CameraPath * cameraPath;


bool buttons[11];
enum buttonConstants { FORWARD = 0, BACKWARD, UP, DOWN, LEFT, RIGHT, EARTH_FORWARD, EARTH_BACKWARD, SCREENSHOT, PRINT_STATE, JUMP };

void myDrawFun();
void myPreSyncFun();
void myEncodeFun();
void myDecodeFun();
void myKeyboardFun(int key, int action);

sgct::SharedVector<char> sharedData;

using namespace earthrendering;
int frameNumber = 0;

int main( int argc, char* argv[] )
{

	ghoul::initialize();

    ghoul::filesystem::File app(argv[0]);
    std::string dirName = app.directoryName();
    //LINFO("Setting starting directory to '" << dirName << "'");
    FileSys.setCurrentDirectory(dirName);
	
	FileSys.registerPathToken("${SHADERS}", "./shaders");
	FileSys.registerPathToken("${DATA}", "./data");
	FileSys.registerPathToken("${CACHE}", "./cache");
	FileSys.registerPathToken("${TEXTURES}", "./textures");

	ghoul::opengl::ShaderPreprocessor::addIncludePath("${SHADERS}");
	ghoul::io::TextureReader::ref().addReader(new ghoul::io::impl::TextureReaderDevIL);

	
	FileSys.createCacheManager(absPath("${CACHE}"), 1);

    std::cout << FileSys.currentDirectory() << std::endl;
        
	if (argc == 1) {
		int newArgc = 3;
		char** newArgv = new char*[newArgc];
		newArgv[0] = argv[0];
		newArgv[1] = "-config";
		newArgv[2] = "./sgct/lowres_fisheye.xml";
		gEngine = new sgct::Engine( newArgc, newArgv );
		delete[] newArgv;
	} else {
		gEngine = new sgct::Engine( argc, argv );
	}

    gEngine->setDrawFunction(myDrawFun);
	gEngine->setPreSyncFunction(myPreSyncFun);
	gEngine->setKeyboardCallbackFunction(myKeyboardFun);
	
	sgct::SharedData::instance()->setEncodeFunction(myEncodeFun);
	sgct::SharedData::instance()->setDecodeFunction(myDecodeFun);

	if( !gEngine->init() )
	{
		delete gEngine;
		return EXIT_FAILURE;
	}

	gEngine->setNearAndFarClippingPlanes(0.1, 200000);

	glClearColor(0.0,0.0,0.0,0.0);
	
	ghoul::systemcapabilities::SystemCapabilities::initialize();
	SysCap.addComponent(new ghoul::systemcapabilities::GeneralCapabilitiesComponent);
	SysCap.addComponent(new ghoul::systemcapabilities::OpenGLCapabilitiesComponent);
	SysCap.detectCapabilities();
	
    camera = new Camera();

	camera->setPosition(glm::vec3(0.0, 0.0, -12.0));
	camera->setDirection(glm::vec3(0.0, 0.0, 1.0));
	camera->setUp(glm::vec3(0.0, 1.0, 1.0));

    renderer = new Renderer();
    earth = new Earth(1000);

	
    if (!earth->initialize()) {
        std::cout << "Could not initialize earth." << std::endl;
        std::cin.get();
        return 0;
    }
	
	sky = new Sky(100);
    if (!sky->initialize()) {
        std::cout << "Could not initialize sky." << std::endl;
        std::cin.get();
        return 0;
    }
	
	sun = new Sun();
	if (!sun->initialize()) {
        std::cout << "Could not initialize sun." << std::endl;
        std::cin.get();
        return 0;
    }

	cameraPath = new CameraPath("${DATA}/keyframes_web.txt");

	sun->setPosition(glm::vec3(5.0, 0.0, -50.0));
	earth->setSunPosition(glm::vec3(5.0, 0.0, -50.0)); // todo: set a good sun position in world coords.


	renderer->addChild(sky);
	renderer->addChild(sun);
    renderer->addChild(earth);

    //renderer->addChild(new Stars());

	// Main loop
	gEngine->render();

	// Clean up (de-allocate)
	delete gEngine;

	delete earth;
	delete sky;
	delete sun;
	delete camera;
	delete cameraPath;
	delete renderer;

	// Exit program
	exit( EXIT_SUCCESS );
}

void myDrawFun()
{
	glm::mat4 mv = gEngine->getCurrentModelViewMatrix();
	glm::mat4 p = gEngine->getCurrentProjectionMatrix();
	camera->setSgctModelViewMatrix(mv);
	camera->setSgctProjectionMatrix(p);

	// float earthTilt = - 23.4 * M_PI / 180.0; // winter
	float earthTilt = - 10.4 * M_PI / 180.0; // lite höst sådär

	// objektet
	// translation
	// rotation (
	// 


/*	float earthRotation = static_cast<float>(frameNumber)*0.01;
	
	glm::mat4 invEarthTranslation = glm::translate(glm::mat4(1.0), glm::vec3(50.0, 0.0, 0.0));
	glm::mat4 invEarthAxisTilt = glm::rotate(glm::mat4(1.0), earthTilt, glm::vec3(0.0, 1.0, 0.0));
	glm::mat4 invEarthSpin = glm::rotate(glm::mat4(1.0), -earthRotation, glm::vec3(0.0, 1.0, 0.0));

	glm::vec3 sunPos = (invEarthSpin * invEarthAxisTilt * invEarthTranslation * glm::vec4(0.0, 0.0, 0.0, 1.0)).xyz();*/
	
	glm::vec3 sunPos = glm::vec3(50000 * std::cos(earthTilt), 50000*std::sin(earthTilt), 0.0);

	earth->setSunPosition(sunPos); // todo: set a good sun position in world coords.
	sun->setPosition(sunPos);

	//earth->setRotation(curr_time * 0.01);
	renderer->render(*camera);
}

void myKeyboardFun(int key, int action) {
if( gEngine->isMaster() )
	{
		if (action != GLFW_PRESS && action != GLFW_RELEASE)  {
			// guard against repeated actions when holding down a key.
			return;
		}
		
		switch( key )
		{
		case GLFW_KEY_UP:
		case 'W':
			buttons[FORWARD] = (action == GLFW_PRESS ? true : false);
			break;

		case GLFW_KEY_DOWN:
		case 'S':
			buttons[BACKWARD] = (action == GLFW_PRESS ? true : false);
			break;

		case GLFW_KEY_LEFT:
		case 'A':
			buttons[LEFT] = (action == GLFW_PRESS ? true : false);
			break;

		case GLFW_KEY_RIGHT:
		case 'D':
			buttons[RIGHT] = (action == GLFW_PRESS ? true : false);
			break;

		case 'R':
			buttons[UP] = (action == GLFW_PRESS ? true : false);
			break;
		case 'F':
			buttons[DOWN] = (action == GLFW_PRESS ? true : false);
			break;
			
		case 'Q':
			buttons[EARTH_BACKWARD] = (action == GLFW_PRESS ? true : false);
			break;
		case 'E':
			buttons[EARTH_FORWARD] = (action == GLFW_PRESS ? true : false);
			break;

		case 'P':
			buttons[SCREENSHOT] = (action == GLFW_PRESS ? true : false);
			break;
		case 'O':
			buttons[PRINT_STATE] = (action == GLFW_PRESS ? true : false);
			break;

		case 'J':
			buttons[JUMP] = (action == GLFW_PRESS ? true : false);
			break;

			
		}


	}
}

void myPreSyncFun()
{
	FileSys.triggerFilesystemEvents();

	frameNumber++;

	if (buttons[SCREENSHOT]) {
		gEngine->takeScreenshot();
		//buttons[SCREENSHOT] = false;
    };



	
	//set the time only on the master
	if( gEngine->isMaster() ) {

		if (buttons[JUMP]) {
			std::cin.clear();
			std::cin.ignore(10000,'\n');
			double sec = 0;
			std::cout << "jump to second: ";
			std::cin >> sec;
			frameNumber = sec * 60;
		}
		
		cameraPath->updateCamera(*camera, *earth, static_cast<float>(frameNumber) / 60);	
		
		if (buttons[PRINT_STATE]) {
			cameraPath->printState(static_cast<float>(frameNumber) / 60);
			buttons[PRINT_STATE] = false;
		}
	}

}

void myEncodeFun()
{
	sgct::SharedData *sd = sgct::SharedData::instance();
	BinaryStream bs; 

	bs << frameNumber;
	bs << camera->serialize();
	bs << earth->serialize();

	sharedData.setVal(bs.data());
	sd->writeVector(&sharedData);
}

void myDecodeFun()
{
	sgct::SharedData *sd = sgct::SharedData::instance();
	sd->readVector(&sharedData);
	BinaryStream bs(sharedData.getVal());

	bs >> frameNumber;
	
	std::vector<char> cameraData;
	bs >> cameraData;
	camera->unserialize(cameraData);

	std::vector<char> earthData;
	bs >> earthData;
	earth->unserialize(earthData);
}
