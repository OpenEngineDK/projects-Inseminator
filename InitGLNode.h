#ifndef _INIT_GL_NODE_
#define _INIT_GL_NODE_

#include <Renderers/IRenderer.h>
#include <Renderers/IRenderNode.h>
#include <Renderers/IRenderingView.h>

using namespace OpenEngine::Renderers;

class InitGLNode : public IRenderNode {
public:
    InitGLNode() {}
    void Apply(IRenderingView* rv) {
        //rv->GetRenderer()->EnableBlending(IRenderer::SRC_COLOR,
                                          IRenderer::ONE,
                                          IRenderer::REVERSE_SUBTRACT);
        VisitSubNodes(*rv);
        //rv->GetRenderer()->DisableBlending();
    }
};

#endif
