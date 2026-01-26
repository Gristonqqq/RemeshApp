#pragma once
#include <QString>
#include "CoreMesh.h"
#include "RemeshParams.h"

class IRemeshStrategy
{
public:
    virtual ~IRemeshStrategy() = default;

    virtual QString name() const = 0;

    virtual bool run(CoreMesh& mesh, const RemeshParams& p, QString* outError = nullptr) = 0;
};
