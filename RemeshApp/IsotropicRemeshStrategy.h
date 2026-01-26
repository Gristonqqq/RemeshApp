#pragma once
#include "IRemeshStrategy.h"

class IsotropicRemeshStrategy final : public IRemeshStrategy
{
public:
    QString name() const override { return "Isotropic Remesh"; }
    bool run(CoreMesh& mesh, const RemeshParams& p, QString* outError = nullptr) override;
};
