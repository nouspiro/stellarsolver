#pragma once

#include <QObject>
#include "structuredefinitions.h"

namespace SSolver
{

//This is the shape used for source extration in sextractor
enum Shape
{
    SHAPE_AUTO,
    SHAPE_CIRCLE,
    SHAPE_ELLIPSE
};

//This gets a string for the Sextractor setting for calculating Flux using ellipses or circles
static QString getShapeString(SSolver::Shape shape)

{
    switch(shape)
    {
        case SHAPE_AUTO:
            return "Auto";
            break;

        case SHAPE_CIRCLE:
            return "Circle";
            break;

        case SHAPE_ELLIPSE:
            return "Ellipse";
            break;
        default:
            return "";
            break;
    }
}

//This is a structure to hold the paths to the external programs for solving.
//This makes the paths easier to access and set
typedef struct
{
    QString confPath;               //Path to the Astrometry Config File
    QString sextractorBinaryPath;   //Path to the Sextractor Program binary
    QString solverPath;             //Path to the Astrometry Solver binary
    QString astapBinaryPath;        //Path to the ASTAP Program binary
    QString wcsPath;                //Path to the WCSInfo binary
} ExternalProgramPaths;

// This is the units used by astrometry.net for the scale for plate solving
typedef enum {DEG_WIDTH,
              ARCMIN_WIDTH,
              ARCSEC_PER_PIX,
              FOCAL_MM
             } ScaleUnits;

// This gets the scale unit string for astrometry.net input
// This should NOT be translated
static QString getScaleUnitString(SSolver::ScaleUnits scaleunit)
{
    switch(scaleunit)
    {
        case DEG_WIDTH:
            return "degwidth";
            break;
        case ARCMIN_WIDTH:
            return "arcminwidth";
            break;
        case ARCSEC_PER_PIX:
            return "arcsecperpix";
            break;
        case FOCAL_MM:
            return "focalmm";
            break;
        default:
            return "";
            break;
    }
}

// This is the list of operations that the Stellarsolver can do.
// You need to set this either directly or using a method before starting the process.
typedef enum { EXTRACT,            //This just sextracts the sources
               EXTRACT_WITH_HFR,   //This sextracts the sources and finds the HFR
               SOLVE                //This solves the image
             } ProcessType;

typedef enum { EXTRACTOR_INTERNAL, //This uses internal SEP to Sextract Sources
               EXTRACTOR_EXTERNAL,  //This uses the external sextractor to Sextract Sources.
               EXTRACTOR_BUILTIN  //This uses whatever default sextraction method the selected solver uses
             } ExtractorType;

typedef enum { SOLVER_STELLARSOLVER,    //This uses the internal build of astrometry.net
               SOLVER_LOCALASTROMETRY,  //This uses an astrometry.net or ANSVR locally on this computer
               SOLVER_ASTAP,            //This uses a local installation of ASTAP
               SOLVER_ONLINEASTROMETRY  //This uses the online astrometry.net or ASTAP
             } SolverType;

//This gets the processType as a string explaining the command StellarSolver is Running
static QString getCommandString(SSolver::ProcessType processType, SSolver::ExtractorType m_ExtractorType,
                                SSolver::SolverType solverType)
{
    QString commandString = "";

    switch(m_ExtractorType)
    {
        case EXTRACTOR_INTERNAL:
            commandString += "Internal ";
            break;

        case EXTRACTOR_EXTERNAL:
            commandString += "External ";
            break;

        case EXTRACTOR_BUILTIN:
            commandString += "Built In ";
            break;
    }

    switch(processType)
    {
        case EXTRACT:
            commandString += "Extractor ";
            break;

        case EXTRACT_WITH_HFR:
            commandString += "Extractor w/HFR ";
            break;

        case SOLVE:
            commandString += "Extractor w/ ";
            break;
    }

    if(processType == SOLVE)
    {
        switch(solverType)
        {
            case SOLVER_STELLARSOLVER:
                commandString += "StellarSolver ";
                break;

            case SOLVER_LOCALASTROMETRY:
                commandString += "local solver ";
                break;

            case SOLVER_ASTAP:
                commandString += "local ASTAP ";
                break;

            case SOLVER_ONLINEASTROMETRY:
                commandString += "online solver ";
                break;
        }
    }
    return commandString;
};


// These are the algorithms used for patallel solving
// When solving an image, this is one of the Parameters
typedef enum {NOT_MULTI,    // This option does not use parallel solving
              MULTI_SCALES, // This option generates multiple threads based on different image scales
              MULTI_DEPTHS, // This option generates multiple threads based on different image "depths"
              MULTI_AUTO    // This option generates multiple threads (or not) automatically based on the algorithm that is best
             } MultiAlgo;

//This gets a string for which Parallel Solving Algorithm we are using
static QString getMultiAlgoString(SSolver::MultiAlgo multi)
{
    switch(multi)
    {
        case NOT_MULTI:
            return "None";
            break;

        case MULTI_SCALES:
            return "Scales";
            break;

        case MULTI_DEPTHS:
            return "Depths";
            break;
        default:
            return "";
            break;
    }
}

// Astrometry.net struct for defining the log level
// It is defined both here and in log.h so that we can set the log level here without including all of log.h.
typedef enum
{
    LOG_OFF,
    LOG_NORMAL,
    LOG_VERBOSE
} SSolverLogLevel;

// Astrometry.net struct for defining the log level
// It is defined both here and in log.h so that we can set the log level here without including all of log.h.
enum logging_level
{
    LOG_NONE,
    LOG_ERROR,
    LOG_MSG,
    LOG_VERB,
    LOG_ALL
};

static QString getLogLevelString(SSolver::logging_level logLevel)
{
    switch(logLevel)
    {
        case LOG_NONE:
            return "None";
            break;

        case LOG_ERROR:
            return "Error";
            break;

        case LOG_MSG:
            return "Message";
            break;

        case LOG_VERB:
            return "Verbose";
            break;

        case LOG_ALL:
            return "All";
            break;

        default:
            return "";
            break;
    }
}

//STELLARSOLVER PARAMETERS
//These are the parameters used by the StellarSolver for both Sextracting and Solving
//The values here are the defaults unless they get changed.
//If you are fine with those defaults, you don't need to set any of them.

class Parameters
{
    public:

        // These are the available parameter profiles that are built into StellarSolver.
        // You can just use them without having to set any parameters yourself.
        typedef enum
        {
            FAST_SOLVING,
            PARALLEL_SOLVING,
            PARALLEL_LARGESCALE,
            PARALLEL_SMALLSCALE,
            ALL_STARS,
            SMALL_STARS,
            MID_STARS,
            BIG_STARS
        } ParametersProfile;

        QString listName = "Default";       // This is the name of this particular profile of options for StellarSolver

        //Sextractor Photometry Parameters
        Shape apertureShape = SHAPE_CIRCLE; // Whether to use the SEP_SUM_ELLIPSE method or the SEP_SUM_CIRCLE method
        double kron_fact = 2.5;             // This sets the Kron Factor for use with the kron radius for flux calculations.
        int subpix = 5;                     // The subpix setting.  The instructions say to make it 5
        double r_min = 3.5;                 // The minimum radius for stars for flux calculations.
        short inflags;                      // Note sure if we need them?

        //Sextractor Extraction Parameters
        double magzero =
            20;                // This is the 'zero' magnitude used for settting the magnitude scale for the stars in the image during sextraction.
        double minarea = 5;                 // This is the minimum area in pixels for a star detection, smaller stars are ignored.
        int deblend_thresh = 32;            // The number of thresholds the intensity range is divided up into.
        double deblend_contrast =
            0.005;    // The percentage of flux a separate peak must # have to be considered a separate object.
        int clean = 1;                      // Attempts to 'clean' the image to remove artifacts caused by bright objects
        double clean_param = 1;             // The cleaning parameter, not sure what it does.
        double fwhm =
            2;                    // A variable to store the fwhm used to generate the conv filter, changing this WILL NOT change the conv filter, you can use the method below to create the conv filter based on the fwhm

        //This is the filter used for convolution. You can create this directly or use the convenience method below.
        QVector<float> convFilter = {0.260856, 0.483068, 0.260856,
                                     0.483068, 0.894573, 0.483068,
                                     0.260856, 0.483068, 0.260856
                                    };

        //Star Filter Parameters
        double maxSize =
            0;                 // The maximum size of stars to include in the final list in pixels based on semi-major and semi-minor axes
        double minSize =
            0;                 // The minimum size of stars to include in the final list in pixels based on semi-major and semi-minor axes
        double maxEllipse =
            0;              // The maximum ratio between the semi-major and semi-minor axes for stars to include (a/b)
        int initialKeep = 1000000;          // Number of stars to process before filtering.
        double keepNum = 0;                 // The number of brightest stars to keep in the list
        double removeBrightest = 0;         // The percentage of brightest stars to remove from the list
        double removeDimmest = 0;           // The percentage of dimmest stars to remove from the list
        double saturationLimit = 0;         // Remove all stars above a certain threshhold percentage of saturation

        //Astrometry Config/Engine Parameters
        MultiAlgo multiAlgorithm = NOT_MULTI;// Algorithm for running multiple threads on possibly multiple cores to solve faster
        bool inParallel =
            true;             // Check the indices in parallel? if the indices you are using take less than 2 GB of space, and you have at least as much physical memory as indices, you want this enabled,
        int solverTimeLimit = 600;          // Give up solving after the specified number of seconds of CPU time
        double minwidth =
            0.1;              // If no scale estimate is given, this is the limit on the minimum field width in degrees.
        double maxwidth =
            180;              // If no scale estimate is given, this is the limit on the maximum field width in degrees.

        //Astrometry Basic Parameters
        bool resort =
            true;                 // Whether to resort the stars based on magnitude NOTE: This is REQUIRED to be true for the filters above
        bool autoDownsample =       // Whether or not to automatically determine the downsample size based on the image size.
                true;
        int downsample =
            1;                 // Factor to use for downsampling the image before SEP for plate solving.  Can speed it up.  This is not used for Source Extraction
        int search_parity = 2;              // Only check for matches with positive/negative parity (default: try both)
        double search_radius = 15;          // Only search in indexes within 'radius' of the field center given by RA and DEC

        //LogOdds Settings
        double logratio_tosolve = log(1e9); // Odds ratio at which to consider a field solved (default: 1e9)
        double logratio_tokeep  = log(1e9); // Odds ratio at which to keep a solution (default: 1e9)
        double logratio_totune  = log(
                                      1e6); // Odds ratio at which to try tuning up a match that isn't good enough to solve (default: 1e6)

        bool operator==(const Parameters &o);

        static QMap<QString, QVariant> convertToMap(Parameters params);
        static Parameters convertFromMap(QMap<QString, QVariant> settingsMap);

    signals:

}; // Parameters

}  // namespace SSolver

