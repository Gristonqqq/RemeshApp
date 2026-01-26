#pragma once
#include "IRemeshStrategy.h"

class DecimateStrategy final : public IRemeshStrategy
{
public:
    QString name() const override { return "Decimate"; }
    bool run(CoreMesh& mesh, const RemeshParams& p, QString* outError = nullptr) override;
};
