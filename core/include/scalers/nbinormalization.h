/* Copyright (c) 2011-2017, NVIDIA CORPORATION. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of NVIDIA CORPORATION nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <basic_types.h>
#include <scalers/scaler.h>

namespace amgx
{

template <class TConfig> class NBinormalizationScaler;

template <class T_Config>
class NBinormalizationScaler_Base : public Scaler<T_Config>
{
    public:
        typedef Scaler<T_Config> Base;
        typedef T_Config TConfig;
        static const AMGX_VecPrecision vecPrec = TConfig::vecPrec;
        static const AMGX_MatPrecision matPrec = TConfig::matPrec;
        static const AMGX_IndPrecision indPrec = TConfig::indPrec;
        typedef AMG<vecPrec, matPrec, indPrec> AMG_Class;
        typedef typename T_Config::MatPrec ValueTypeA;
        typedef typename T_Config::VecPrec ValueTypeB;
        typedef Vector<T_Config> VVector;
        typedef Vector<TemplateConfig<AMGX_host, vecPrec, matPrec, indPrec> > Vector_h;
        typedef typename Matrix<TConfig>::MVector MVector;

    private:
        virtual void setup(Matrix<T_Config> &A) = 0;
        virtual void scaleMatrix(Matrix<T_Config> &A, ScaleDirection scaleOrUnscale) = 0;
        virtual void scaleVector(VVector &v, ScaleDirection scaleOrUnscale, ScaleSide leftOrRight) = 0;

    public:
        // Constructor
        NBinormalizationScaler_Base( AMG_Config &cfg, const std::string &cfg_scope) {};
        // Destructor
        ~NBinormalizationScaler_Base() {};
};


// ----------------------------
//  specialization for host
// ----------------------------

template <AMGX_VecPrecision t_vecPrec, AMGX_MatPrecision t_matPrec, AMGX_IndPrecision t_indPrec>
class NBinormalizationScaler< TemplateConfig<AMGX_host, t_vecPrec, t_matPrec, t_indPrec> > : public NBinormalizationScaler_Base< TemplateConfig<AMGX_host, t_vecPrec, t_matPrec, t_indPrec> >
{

    public:
        typedef TemplateConfig<AMGX_host, t_vecPrec, t_matPrec, t_indPrec> TConfig_h;
        typedef Matrix<TConfig_h> Matrix_h;
        typedef typename TConfig_h::MatPrec ValueTypeA;
        typedef typename TConfig_h::VecPrec ValueTypeB;
        typedef Vector<TConfig_h> VVector;
        typedef typename TConfig_h::IndPrec IndexType;
        NBinormalizationScaler(AMG_Config &cfg, const std::string &cfg_scope) : NBinormalizationScaler_Base<TConfig_h>(cfg, cfg_scope) {};
        VVector left_scale;
        VVector right_scale;
        ValueTypeB norm_coef;
        bool scaled_before;

    private:
        void setup(Matrix_h &A);
        void scaleMatrix(Matrix_h &A, ScaleDirection scaleOrUnscale);
        void scaleVector(VVector &v,  ScaleDirection scaleOrUnscale, ScaleSide leftOrRight);
};

// ----------------------------
//  specialization for device
// ----------------------------

template <AMGX_VecPrecision t_vecPrec, AMGX_MatPrecision t_matPrec, AMGX_IndPrecision t_indPrec>
class NBinormalizationScaler< TemplateConfig<AMGX_device, t_vecPrec, t_matPrec, t_indPrec> > : public NBinormalizationScaler_Base< TemplateConfig<AMGX_device, t_vecPrec, t_matPrec, t_indPrec> >
{
    public:
        typedef TemplateConfig<AMGX_device, t_vecPrec, t_matPrec, t_indPrec> TConfig_d;
        typedef Matrix<TConfig_d> Matrix_d;
        typedef typename TConfig_d::MatPrec ValueTypeA;
        typedef typename TConfig_d::VecPrec ValueTypeB;
        typedef typename TConfig_d::IndPrec IndexType;
        typedef Vector<TConfig_d> VVector;
        typedef typename Matrix_d ::IVector IVector;

        NBinormalizationScaler(AMG_Config &cfg, const std::string &cfg_scope) : NBinormalizationScaler_Base<TConfig_d>(cfg, cfg_scope) {};
        VVector left_scale;
        VVector right_scale;
        ValueTypeB norm_coef;
        bool scaled_before;

    private:
        void setup(Matrix_d &A);
        void scaleMatrix(Matrix_d &A, ScaleDirection scaleOrUnscale);
        void scaleVector(VVector &v,  ScaleDirection scaleOrUnscale, ScaleSide leftOrRight);
};

template <class T_Config>
class NBinormalizationScalerFactory : public ScalerFactory<T_Config>
{
    public:
        Scaler<T_Config> *create( AMG_Config &cfg, const std::string &cfg_scope)
        {
            return new NBinormalizationScaler<T_Config>( cfg, cfg_scope );
        }
};

} // namespace amgx