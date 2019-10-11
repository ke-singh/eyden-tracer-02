#pragma once

#include "ShaderFlat.h"

class CScene;

class CShaderPhong : public CShaderFlat
{
public:
    /**
     * @brief Constructor
     * @param scene Reference to the scene
     * @param color The color of the object
     * @param ka The ambient coefficient
     * @param kd The diffuse reflection coefficients
     * @param ks The specular refelection coefficients
     * @param ke The shininess exponent
     */
    CShaderPhong(CScene& scene, Vec3f color, float ka, float kd, float ks, float ke)
        : CShaderFlat(color)
        , m_scene(scene)
        , m_ka(ka)
        , m_kd(kd)
        , m_ks(ks)
        , m_ke(ke)
    {}
    virtual ~CShaderPhong(void) = default;

    virtual Vec3f Shade(const Ray& ray) const override
    {
        Vec3f firstTerm = m_ka * CShaderFlat::Shade(ray); // the first term of the equation
        Ray nRay;
        std::optional<Vec3f> nRadiance;
        Vec3f sumInSecondTerm = 0;
        Vec3f sumInThirdTerm = 0;
        Vec3f reflectionLength;
        for (const auto& light : m_scene.m_vpLights) {
            nRay.org = ray.org + ray.t * ray.dir;
            nRadiance = light->Illuminate(nRay);
            nRay.t = std::numeric_limits<float>::infinity();
            if (!m_scene.Occluded(nRay)) {
                sumInSecondTerm += * nRadiance * max(0.0f, nRay.dir.dot(ray.hit->GetNormal(ray)));
                reflectionLength = nRay.dir - 2 * (nRay.dir.dot(ray.hit->GetNormal(ray))) * ray.hit->GetNormal(ray);
                sumInThirdTerm += * nRadiance * pow(max(0.0f, ray.dir.dot(reflectionLength)), m_ke);
            }
        }
        Vec3f secondTerm = m_kd * sumInSecondTerm.mul(CShaderFlat::Shade(ray));
        Vec3f thirdTerm = m_ks * RGB(1, 1, 1).mul(sumInThirdTerm);
        return firstTerm + secondTerm + thirdTerm;

    }


private:
    CScene& m_scene;
    float 	m_ka;    ///< ambient coefficient
    float 	m_kd;    ///< diffuse reflection coefficients
    float 	m_ks;    ///< specular refelection coefficients
    float 	m_ke;    ///< shininess exponent
};
