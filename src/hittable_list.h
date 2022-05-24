// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2022 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include <memory>
#include <vector>

/**
 * @todo write docs
 */


class hittable_list : public Hittable
{
public:
    hittable_list(){}
    hittable_list(std::shared_ptr<Hittable> object);

    void clear();
    void add(std::shared_ptr<Hittable> object);

    virtual bool hit ( const Ray & r, float t_min, float t_max, hitrecord & rec ) const override;

public:
    std::vector<std::shared_ptr<Hittable>> objects;
};

#endif // HITTABLE_LIST_H
