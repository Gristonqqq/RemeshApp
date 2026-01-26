#pragma once
#include "IRemeshStrategy.h"

class SubdivideStrategy final : public IRemeshStrategy
{
public:
    QString name() const override { return "Subdivide"; }
    bool run(CoreMesh& mesh, const RemeshParams& p, QString* outError = nullptr) override;

private:
    static bool subdivideOnce(CoreMesh& mesh, QString* outError);
};
