#include "core.h"


IClock::IClock(float startTimeSeconds) { clk = new tClock(startTimeSeconds); };
IClock::~IClock(void) { delete clk; };
void IClock::init() { clk->init(); };
void IClock::startMeasure() { clk->startMeasure(); };
float IClock::checkAndSwapMeasure() { return clk->checkAndSwapMeasure(); };
unsigned long long IClock::getTimeCycles() { return clk->getTimeCycles(); };
float IClock::cyclesToSeconds(unsigned long long dt) { return clk->cyclesToSeconds(dt); };
void IClock::setPaused(bool paused) { clk->setPaused(paused); };
void IClock::setTimeScale(float scale) { clk->setTimeScale(scale); };


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
/*void ISoftwareRasterizer::initBuffers() { rst->initBuffers(); };
void ISoftwareRasterizer::setOutBuffer(int* displayBuf) { rst->setOutBuffer(displayBuf); };
int* ISoftwareRasterizer::bufColor() { return rst->bufColor(); };
void ISoftwareRasterizer::sGLFreeOutput() { rst->sGLFreeOutput(); };
void ISoftwareRasterizer::sGLBufferData(int size, IVertexData* ivrtx) { rst->sGLBufferData(size, ivrtx->getVrtxDataPtr()); };
void ISoftwareRasterizer::sGLDrawElements(int numElem) { rst->sGLDrawElements(numElem); };*/



ISoftwareRenderer::ISoftwareRenderer(int _width, int _height, int _pxl) { rndr = new tSoftwareRenderer(_width, _height, _pxl); };
ISoftwareRenderer::~ISoftwareRenderer(void) { delete rndr; };
void ISoftwareRenderer::startUp() { rndr->startUp(); };
void ISoftwareRenderer::shutDown() { rndr->shutDown(); };
void ISoftwareRenderer::setWorldPtr(tScene* _wrld) { rndr->setWorldPtr(_wrld); }
void ISoftwareRenderer::setCamPtr(tCameraObject* _cam) { rndr->setCamPtr(_cam); }
void ISoftwareRenderer::render() { rndr->render(); };
int ISoftwareRenderer::width() { return rndr->getWidth(); };
int ISoftwareRenderer::height() { return rndr->getHeight(); };
//void ISoftwareRenderer::updateWorld() { rndr->updateWorld(); };
void ISoftwareRenderer::displayResult() { rndr->displayResult(); };

