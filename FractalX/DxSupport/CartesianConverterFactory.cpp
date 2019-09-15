#include "stdafx.h"
#include "CartesianConverterFactory.h"

#include "TraceParams.h"

namespace DXF
{
	namespace 
	{
		std::function <double(double theta, double phi)> GetComponent(double k, TrigOption thetaOption, TrigOption phiOption)
		{
			switch (thetaOption)
			{
				case TrigOption::Sin:
				{
					switch (phiOption)
					{
						case TrigOption::Sin:
						{
							return [k](double theta, double phi)
							{
								return k * sin(theta) * sin(phi);
							};
						}
						case TrigOption::Cos:
						{
							return [k](double theta, double phi)
							{
								return k * sin(theta) * cos(phi);
							};
						}
						case TrigOption::Tan:
						{
							return [k](double theta, double phi)
							{
								return k * sin(theta) * tan(phi);
							};
						}
						case TrigOption::None:
						default:
						{
							return [k](double theta, double)
							{
								return k * sin(theta);
							};
						}
					}
				}
				case TrigOption::Cos:
				{
					switch (phiOption)
					{
						case TrigOption::Sin:
						{
							return [k](double theta, double phi)
							{
								return k * cos(theta) * sin(phi);
							};
						}
						case TrigOption::Cos:
						{
							return [k](double theta, double phi)
							{
								return k * cos(theta) * cos(phi);
							};
						}
						case TrigOption::Tan:
						{
							return [k](double theta, double phi)
							{
								return k * cos(theta) * tan(phi);
							};
						}
						case TrigOption::None:
						default:
						{
							return [k](double theta, double)
							{
								return k * cos(theta);
							};
						}
					}
				}
				case TrigOption::Tan:
				{
					switch (phiOption)
					{
						case TrigOption::Sin:
						{
							return [k](double theta, double phi)
							{
								return k * tan(theta) * sin(phi);
							};
						}
						case TrigOption::Cos:
						{
							return [k](double theta, double phi)
							{
								return k * tan(theta) * cos(phi);
							};
						}
						case TrigOption::Tan:
						{
							return [k](double theta, double phi)
							{
								return k * tan(theta) * tan(phi);
							};
						}
						case TrigOption::None:
						default:
						{
							return [k](double theta, double)
							{
								return k * tan(theta);
							};
						}
					}
				}
				case TrigOption::None:
				default:
				{
					switch (phiOption)
					{
						case TrigOption::Sin:
						{
							return [k](double, double phi)
							{
								return k * sin(phi);
							};
						}
						case TrigOption::Cos:
						{
							return [k](double, double phi)
							{
								return k * cos(phi);
							};
						}
						case TrigOption::Tan:
						{
							return [k](double, double phi)
							{
								return k * tan(phi);
							};
						}
						case TrigOption::None:
						default:
						{
							return [k](double, double)
							{
								return k;
							};
						}
					}
				}
			}
		}
	}

	// function params are - Vector3Double& z, const double& zr, double& theta, double& phi
	std::function<void(Vector3Double&, const double&, const double&, const double&)> CreateConverter(const CartesianConverterGroup& requestParams)
	{
		auto pfX = GetComponent(requestParams.MultiplerX, requestParams.ThetaOptionX, requestParams.PhiOptionX);
		auto pfY = GetComponent(requestParams.MultiplerY, requestParams.ThetaOptionY, requestParams.PhiOptionY);
		auto pfZ = GetComponent(requestParams.MultiplerZ, requestParams.ThetaOptionZ, requestParams.PhiOptionZ);

		return [pfX, pfY, pfZ](Vector3Double& p, const double& zr, double theta, double phi)
		{
			p = zr * Vector3Double(pfX(theta,phi), pfY(theta,phi), pfZ(theta, phi));
		};
	}
}