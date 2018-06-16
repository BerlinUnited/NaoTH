#include "AdaptiveAutoExposure.h"

AdaptiveAutoExposure::AdaptiveAutoExposure()
{   
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:draw_weights", "Draw weight grid in image", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:draw_weights_wrong", "Draw weight grid in image in cool/wrong fashion", false);
}

void AdaptiveAutoExposure::execute()
{

    if(getCommonCameraSettingsRequest().isActive && getCommonCameraSettingsRequest().autoExposition) 
    {

        if(getCommonCameraSettingsRequest().autoExpositionMethod == "averageY") 
        {
            getCameraSettingsRequest().autoExpositionAlgorithm = 0;
            getCameraSettingsRequestTop().autoExpositionAlgorithm = 0;
        }
        else
        {
            // apply weight table based auto exposition
            getCameraSettingsRequest().autoExpositionAlgorithm = 1;
            getCameraSettingsRequestTop().autoExpositionAlgorithm = 1;

            const std::uint8_t onVal = 100;
            // include all per default
            getCameraSettingsRequest().setAutoExposureWeights(onVal);
            getCameraSettingsRequestTop().setAutoExposureWeights(onVal);

            if(getCommonCameraSettingsRequest().autoExpositionMethod == "dortmund") 
            {
                getCameraSettingsRequest().reset();
                getCameraSettingsRequestTop().reset();

                getCameraSettingsRequestTop().autoExposureWeights[0][0] = 25;
                getCameraSettingsRequestTop().autoExposureWeights[0][1] = 25;
                getCameraSettingsRequestTop().autoExposureWeights[0][2] = 25;
                getCameraSettingsRequestTop().autoExposureWeights[0][3] = 25;
                getCameraSettingsRequestTop().autoExposureWeights[0][4] = 25;

                getCameraSettingsRequestTop().autoExposureWeights[1][0] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[1][1] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[1][2] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[1][3] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[1][4] = 100;

                getCameraSettingsRequestTop().autoExposureWeights[2][0] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[2][1] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[2][2] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[2][3] = 100;
                getCameraSettingsRequestTop().autoExposureWeights[2][4] = 100;

                getCameraSettingsRequestTop().autoExposureWeights[3][0] = 75;
                getCameraSettingsRequestTop().autoExposureWeights[3][1] = 75;
                getCameraSettingsRequestTop().autoExposureWeights[3][2] = 75;
                getCameraSettingsRequestTop().autoExposureWeights[3][3] = 75;
                getCameraSettingsRequestTop().autoExposureWeights[3][4] = 75;
            
                getCameraSettingsRequest().autoExposureWeights[0][0] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][1] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][2] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][3] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][4] = 25;

                getCameraSettingsRequest().autoExposureWeights[1][0] = 50;
                getCameraSettingsRequest().autoExposureWeights[1][1] = 75;
                getCameraSettingsRequest().autoExposureWeights[1][2] = 75;
                getCameraSettingsRequest().autoExposureWeights[1][3] = 75;
                getCameraSettingsRequest().autoExposureWeights[1][4] = 50;

                getCameraSettingsRequest().autoExposureWeights[2][0] = 75;
                getCameraSettingsRequest().autoExposureWeights[2][1] = 75;
                getCameraSettingsRequest().autoExposureWeights[2][2] = 75;
                getCameraSettingsRequest().autoExposureWeights[2][3] = 75;
                getCameraSettingsRequest().autoExposureWeights[2][4] = 75;

                getCameraSettingsRequest().autoExposureWeights[3][0] = 25;
                getCameraSettingsRequest().autoExposureWeights[3][1] = 25;
                getCameraSettingsRequest().autoExposureWeights[3][2] = 25;
                getCameraSettingsRequest().autoExposureWeights[3][3] = 25;
                getCameraSettingsRequest().autoExposureWeights[3][4] = 25;

            }
            else if(getCommonCameraSettingsRequest().autoExpositionMethod == "centerlines3") 
            {
                getCameraSettingsRequest().reset();
                getCameraSettingsRequestTop().reset();

                getCameraSettingsRequestTop().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][3] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[2][1] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[2][2] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[2][3] = onVal;
    
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][3] = onVal; 
            } 
            else if(getCommonCameraSettingsRequest().autoExpositionMethod == "centerlines2") 
            {
                getCameraSettingsRequest().reset();
                getCameraSettingsRequestTop().reset();

                getCameraSettingsRequestTop().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequestTop().autoExposureWeights[3][3] = onVal;
        
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][3] = onVal;
            
            }
            executeDebugDrawings(onVal);
        }
    }
}

void AdaptiveAutoExposure::executeDebugDrawings(std::uint8_t onVal)
{
    DEBUG_REQUEST("AdaptiveAutoExposure:draw_weights",
            IMAGE_DRAWING_CONTEXT;
            int gridWidth = getImage().width() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
            int gridHeight = getImage().height() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
            for(std::size_t x = 0; x < CameraSettings::AUTOEXPOSURE_GRID_SIZE; x++) {
                for(std::size_t y=0; y < CameraSettings::AUTOEXPOSURE_GRID_SIZE; y++) {
                    double w_bottom = (double) getCameraSettingsRequest().autoExposureWeights[y][x];
                    double w_top = (double) getCameraSettingsRequestTop().autoExposureWeights[y][x];
                    
                    CANVAS("ImageTop");

                    double val = w_top / (double) onVal; 
                    PEN("FF0000", 1);
                    BOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    PEN(Color(1.0, 0.0, 0.0, 1.0-val).toString(), 5);
                    // draw a cross where the alpha is inverse to the wieght
                    LINE((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );
                    LINE((int) ((x+1)*gridWidth), 
                        (int) (y*gridHeight),
                        (int) (x*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    CANVAS("ImageBottom");

                    val = w_bottom / (double) onVal; 
                    PEN("FF0000", 1);
                    BOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    PEN(Color(1.0, 0.0, 0.0, 1.0-val).toString(), 5);
                    // draw a cross where the alpha is inverse to the wieght
                    LINE((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );
                    LINE((int) ((x+1)*gridWidth), 
                        (int) (y*gridHeight),
                        (int) (x*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );
                    
                }
            }        
        );

        DEBUG_REQUEST("AdaptiveAutoExposure:draw_weights_wrong",
            IMAGE_DRAWING_CONTEXT;
            int gridWidth = getImage().width() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
            int gridHeight = getImage().height() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
            for(std::size_t x = 0; x < CameraSettings::AUTOEXPOSURE_GRID_SIZE; x++) {
                for(std::size_t y=0; y < CameraSettings::AUTOEXPOSURE_GRID_SIZE; y++) {

                    double w_bottom = (double) getCameraSettingsRequest().autoExposureWeights[y][x];
                    double w_top = (double) getCameraSettingsRequestTop().autoExposureWeights[y][x];

                    CANVAS("ImageTop");
                    double val = w_top / (double) onVal; 
                    PEN("000000", 1);
                    BOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    PEN(Color(val, val, val, 0.5).toString(), 1);
                    FILLBOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    CANVAS("ImageBottom");
                    val = w_bottom / (double) onVal; 
                    PEN("000000", 1);
                    BOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );

                    PEN(Color(val, val, val, 0.5).toString(), 1);
                    FILLBOX((int) (x*gridWidth), 
                        (int) (y*gridHeight),
                        (int) ((x+1)*gridWidth),
                        (int) ((y+1)*gridHeight)
                    );
                    
                }
            }        
        );
}