#ifndef %CLASSNAME%_H
#define %CLASSNAME%_H

#include <QObject>

#include "IPL_plugininterface.h"
#include "IPL_global.h"
#include "IPLProcess.h"

/*!
 * \The plugin container class
 */
class %CLASSNAME%Plugin : public QObject, PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "ch.bfh.cpvrlab.ImagePlay.PluginInterface/1.0")
    Q_INTERFACES(PluginInterface)
public:
    QString name()      { return QString("%CLASSNAME%");}
    QString author()    { return QString("%AUTHOR%");}
    QString version()   { return QString("1.0");}
    IPLProcess* getProcess()
    {
        _process = new %CLASSNAME%Plugin::%CLASSNAME%;
        return _process;
    }
private:
    IPLProcess* _process;

    /*!
     * \ The process implementation
     */

    class %CLASSNAME% : public IPLClonableProcess<%CLASSNAME%>
    {
    public:
        %CLASSNAME%() : IPLClonableProcess() { init(); }
        ~%CLASSNAME%()  { destroy(); }

        void init()
        {
            // init
            _result     = NULL;

            // basic settings
            setClassName("%CLASSNAME%Plugin::%CLASSNAME%");
            setTitle("%TITLE%");
            setCategory(IPLProcess::%CATEGORY%);
			setOpenCVSupport(IPLOpenCVSupport::%OPENCV%);
            setDescription("%DESCRIPTION%");
            setKeywords("%KEYWORDS%");

            // inputs and outputs
            addInput("Image", IPLData::IMAGE_COLOR);
            addOutput("Image", IPLImage::IMAGE_COLOR);

            // properties
            addProcessPropertyDouble("factor", "Factor", "", IPL_DOUBLE_SLIDER, 0.2, 0.0, 1.0);
        }

        void destroy()
        {
            delete _result;
        }

        bool processInputData(IPLImage* input, int, bool useOpenCV)
        {
            // delete previous result
            delete _result;
            _result = NULL;

            int width = input->width();
            int height = input->height();
            _result = new IPLImage(input->type(), width, height);

            // get properties
            double factor = getProcessPropertyDouble("factor");

            int progress = 0;
            int maxProgress = input->height() * input->getNumberOfPlanes();
            int nrOfPlanes = input->getNumberOfPlanes();

            #pragma omp parallel for
            for( int planeNr=0; planeNr < nrOfPlanes; planeNr++ )
            {
                IPLImagePlane* plane = input->plane( planeNr );
                IPLImagePlane* newplane = _result->plane( planeNr );
                for(int y=0; y<height; y++)
                {
                    // progress
                    notifyProgressEventHandler(100*progress++/maxProgress);
                    for(int x=0; x<width; x++)
                    {
                        newplane->p(x,y) = factor * plane->p(x,y);
                    }
                }
            }
            return true;
        }

        IPLData* getResultData(int)
        {
            return _result;
        }

    private:
        IPLImage*               _result;
    };
};

#endif // %CLASSNAME%_H

