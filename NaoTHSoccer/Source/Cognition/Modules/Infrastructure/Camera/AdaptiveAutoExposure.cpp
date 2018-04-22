#include "AdaptiveAutoExposure.h"

AdaptiveAutoExposure::AdaptiveAutoExposure()
{   
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:devils", "...", true);

    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:wide_1_line", "...", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:wide_2_line", "...", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:wide_3_line", "...", false);

    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:centered_1_line", "...", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:centered_2_line", "...", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:centered_3_line", "...", false);

    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:draw_weights", "Draw weight grid in image", false);
    DEBUG_REQUEST_REGISTER("AdaptiveAutoExposure:draw_weights_wrong", "Draw weight grid in image in cool/wrong fashion", false);
}

void AdaptiveAutoExposure::execute()
{
    execute(CameraInfo::Bottom);
    execute(CameraInfo::Top);
}

void AdaptiveAutoExposure::execute(CameraInfo::CameraID id)
{
    cameraID = id;

    const unsigned int onVal = 100;
    // include all
    for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
      for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
          getCameraSettingsRequest().autoExposureWeights[i][j] = onVal; 
      }
    }
    // only apply the non-default values if maching auto exposition algorithm is enabled
    if (getCameraSettingsRequest().autoExposition 
        && getCameraSettingsRequest().autoExpositionAlgorithm == 1)
    {

        DEBUG_REQUEST("AdaptiveAutoExposure:devils",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[0][0] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][1] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][2] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][3] = 25;
                getCameraSettingsRequest().autoExposureWeights[0][4] = 25;

                getCameraSettingsRequest().autoExposureWeights[1][0] = 100;
                getCameraSettingsRequest().autoExposureWeights[1][1] = 100;
                getCameraSettingsRequest().autoExposureWeights[1][2] = 100;
                getCameraSettingsRequest().autoExposureWeights[1][3] = 100;
                getCameraSettingsRequest().autoExposureWeights[1][4] = 100;

                getCameraSettingsRequest().autoExposureWeights[2][0] = 100;
                getCameraSettingsRequest().autoExposureWeights[2][1] = 100;
                getCameraSettingsRequest().autoExposureWeights[2][2] = 100;
                getCameraSettingsRequest().autoExposureWeights[2][3] = 100;
                getCameraSettingsRequest().autoExposureWeights[2][4] = 100;

                getCameraSettingsRequest().autoExposureWeights[3][0] = 75;
                getCameraSettingsRequest().autoExposureWeights[3][1] = 75;
                getCameraSettingsRequest().autoExposureWeights[3][2] = 75;
                getCameraSettingsRequest().autoExposureWeights[3][3] = 75;
                getCameraSettingsRequest().autoExposureWeights[3][4] = 75;
            } else {
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

            
        );

        DEBUG_REQUEST("AdaptiveAutoExposure:wide_1_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][4] = onVal;
            } else {
                getCameraSettingsRequest().autoExposureWeights[0][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][4] = onVal;
            }

            
        );
        DEBUG_REQUEST("AdaptiveAutoExposure:wide_2_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[3][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][4] = onVal;
            } else {
                getCameraSettingsRequest().autoExposureWeights[0][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[1][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][4] = onVal;
            }

            
        );
        DEBUG_REQUEST("AdaptiveAutoExposure:wide_3_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[3][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[2][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][4] = onVal;

            } else {

                getCameraSettingsRequest().autoExposureWeights[0][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[1][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][4] = onVal;

                getCameraSettingsRequest().autoExposureWeights[2][0] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][4] = onVal;
            }            
        );

        DEBUG_REQUEST("AdaptiveAutoExposure:centered_1_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
            } else {
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
            }

            
        );
        DEBUG_REQUEST("AdaptiveAutoExposure:centered_2_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][3] = onVal;
            } else {
                getCameraSettingsRequest().autoExposureWeights[0][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[0][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[1][3] = onVal;
            }

            
        );
        DEBUG_REQUEST("AdaptiveAutoExposure:centered_3_line",
            for(std::size_t i = 0; i < CameraSettings::AUTOEXPOSURE_GRID_SIZE; i++) {
                for(std::size_t j=0; j < CameraSettings::AUTOEXPOSURE_GRID_SIZE; j++) {
                    getCameraSettingsRequest().autoExposureWeights[i][j] = 0; 
                }
            }
            if(cameraID == CameraInfo::Top) {
                getCameraSettingsRequest().autoExposureWeights[4][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[4][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[3][3] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][1] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][2] = onVal;
                getCameraSettingsRequest().autoExposureWeights[2][3] = onVal;
            } else {
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
        );
    }

    DEBUG_REQUEST("AdaptiveAutoExposure:draw_weights",
        IMAGE_DRAWING_CONTEXT;
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        int gridWidth = getImage().width() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
        int gridHeight = getImage().height() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
        for(std::size_t x = 0; x < CameraSettings::AUTOEXPOSURE_GRID_SIZE; x++) {
            for(std::size_t y=0; y < CameraSettings::AUTOEXPOSURE_GRID_SIZE; y++) {
                double val = (double) (getCameraSettingsRequest().autoExposureWeights[y][x]) / (double) onVal; 
                PEN("FF0000", 1);
                BOX((int) (x*gridWidth), 
                    (int) (y*gridHeight),
                    (int) ((x+1)*gridWidth),
                    (int) ((y+1)*gridHeight)
                );

                Color c(1.0, 0.0, 0.0, 1.0-val);
                PEN(c.toString(), 5);
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
        CANVAS(((cameraID == CameraInfo::Top)?"ImageTop":"ImageBottom"));
        int gridWidth = getImage().width() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
        int gridHeight = getImage().height() / CameraSettings::AUTOEXPOSURE_GRID_SIZE;
        for(std::size_t x = 0; x < CameraSettings::AUTOEXPOSURE_GRID_SIZE; x++) {
            for(std::size_t y=0; y < CameraSettings::AUTOEXPOSURE_GRID_SIZE; y++) {
                double val = (double) (getCameraSettingsRequest().autoExposureWeights[y][x]) / (double) onVal; 
                PEN("000000", 1);
                BOX((int) (x*gridWidth), 
                    (int) (y*gridHeight),
                    (int) ((x+1)*gridWidth),
                    (int) ((y+1)*gridHeight)
                );

                Color c(val, val, val, 0.5);
                PEN(c.toString(), 1);
                FILLBOX((int) (x*gridWidth), 
                    (int) (y*gridHeight),
                    (int) ((x+1)*gridWidth),
                    (int) ((y+1)*gridHeight)
                );
                
            }
        }        
    );
}