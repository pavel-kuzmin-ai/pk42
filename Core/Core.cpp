#include "core.h"


IClock::IClock(float startTimeSeconds) { clk = new tClock(startTimeSeconds); };
IClock::~IClock(void) { delete clk; };
void IClock::init() { clk->init(); };
void IClock::startMeasure() { clk->startMeasure(); };
void IClock::checkAndSwapMeasure() { clk->checkAndSwapMeasure(); };
tClock* IClock::getClk() { return clk; };
float IClock::calcDeltaSeconds(IClock other) { return clk->calcDeltaSeconds(*other.getClk()); };


IConsoleScreen::IConsoleScreen(int _width, int _height, int _pixelSize) { scr = new tConsoleScreen(_width, _height, _pixelSize); };
IConsoleScreen::~IConsoleScreen(void) { delete scr; };
void IConsoleScreen::startUp() { scr->startUp(); };
int IConsoleScreen::screenWidth() { return scr->screenWidth(); };
int IConsoleScreen::screenHeight() { return scr->screenWidth(); };
int* IConsoleScreen::bufRGB() { return scr->bufRGB(); };
void IConsoleScreen::updateConsoleBuffer() { scr->updateConsoleBuffer(); };
void IConsoleScreen::show() { scr->show(); };

ISoftwareRasterizer::ISoftwareRasterizer(int _width, int _height) { rst = new tSoftwareRasterizer(_width, _height); };
ISoftwareRasterizer::~ISoftwareRasterizer(void) { delete rst; };
void ISoftwareRasterizer::initBuffers() { rst->initBuffers(); };
void ISoftwareRasterizer::setOutBuffer(int* displayBuf) { rst->setOutBuffer(displayBuf); };
int* ISoftwareRasterizer::bufColor() { return rst->bufColor(); };
void ISoftwareRasterizer::sGLFreeOutput() { rst->sGLFreeOutput(); };
void ISoftwareRasterizer::sGLBufferData(int size, IVertexData* ivrtx) { rst->sGLBufferData(size, ivrtx->getVrtxDataPtr()); };
void ISoftwareRasterizer::sGLDrawElements(int numElem) { rst->sGLDrawElements(numElem); };

IVertexData::IVertexData() {};
IVertexData::IVertexData(vec3<float> c) { vrtx = new tVertexData(c); };
IVertexData::IVertexData(float x, float y, float z) { vrtx = new tVertexData(x, y, z); };;
IVertexData::~IVertexData(void) {};
void IVertexData::translate(float dx, float dy, float dz) { vrtx->translate(dx, dy, dz); };
tVertexData* IVertexData::getVrtxDataPtr() { return vrtx; };