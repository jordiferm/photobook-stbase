/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-11-18
 * Description : a class to apply ICC color correction to image.
 *
 * Copyright (C) 2005-2006 by F.J. Cruz <fj.cruz@supercable.es>
 * Copyright (C) 2005-2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "icctransform.h"

// Qt includes.

#include <QDataStream>
#include <QFile>
#include <QtCore/QVarLengthArray>
#include <QtDebug>


// Lcms includes.
#ifdef ST_ICC_SUPPORT
#include <lcms.h>
#if LCMS_VERSION < 114
#define cmsTakeCopyright(profile) "Unknown"
#endif // LCMS_VERSION < 114
#endif

namespace Digikam
{

	class IccTransformPriv
	{
	public:

		IccTransformPriv()
		{
			has_embedded_profile = false;
			do_proof_profile     = false;
		}

		bool       do_proof_profile;
		bool       has_embedded_profile;

		QByteArray embedded_profile;
		QByteArray input_profile;
		QByteArray output_profile;
		QByteArray proof_profile;
	};

	IccTransform::IccTransform()
			: d(new IccTransformPriv)
	{
#ifdef ST_ICC_SUPPORT
		cmsErrorAction(LCMS_ERROR_SHOW);
#endif
	}

	IccTransform::~IccTransform()
	{
		delete d;
	}

	bool IccTransform::hasInputProfile()
	{
		return !(d->input_profile.isEmpty());
	}

	bool IccTransform::hasOutputProfile()
	{
		return !(d->output_profile.isEmpty());
	}

	QByteArray IccTransform::embeddedProfile() const
	{
		return d->embedded_profile;
	}

	QByteArray IccTransform::inputProfile() const
	{
		return d->input_profile;
	}

	QByteArray IccTransform::outputProfile() const
	{
		return d->output_profile;
	}

	QByteArray IccTransform::proofProfile() const
	{
		return d->proof_profile;
	}

	void IccTransform::getTransformType(bool do_proof_profile)
	{
		if (do_proof_profile)
		{
			d->do_proof_profile = true;
		}
		else
		{
			d->do_proof_profile = false;
		}
	}

	void IccTransform::getEmbeddedProfile(const DImg& image)
	{
		if (!image.getICCProfil().isNull())
		{
			d->embedded_profile     = image.getICCProfil();
			d->has_embedded_profile = true;
		}
	}

	QString IccTransform::getProfileDescription(const QString& profile)
	{
#ifdef ST_ICC_SUPPORT
		cmsHPROFILE _profile = cmsOpenProfileFromFile(QFile::encodeName(profile), "r");
		QString _description = cmsTakeProductDesc(_profile);
		cmsCloseProfile(_profile);
		return _description;
#else
		return "NO ICC SUPPORT";
#endif
	}

	int IccTransform::getRenderingIntent()
	{
		//TODO: Use QSettings
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group = config->group(QString("Color Management"));
		return group.readEntry("RenderingIntent", 0);*/
		return 0;
	}

	bool IccTransform::getUseBPC()
	{
		//TODO: Use QSettings
		/*KSharedConfig::Ptr config = KGlobal::config();
		KConfigGroup group = config->group(QString("Color Management"));
		return group.readEntry("BPCAlgorithm", false);*/
		return false;
	}

	QByteArray IccTransform::loadICCProfilFile(const QString& filePath)
	{
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly))
			return QByteArray();

		QByteArray data;
		data.resize(file.size());
		QDataStream stream(&file);
		stream.readRawData(data.data(), data.size());
		file.close();
		return data;
	}

	void IccTransform::setProfiles(const QString& input_profile, const QString& output_profile)
	{
		d->input_profile  = loadICCProfilFile(input_profile);
		d->output_profile = loadICCProfilFile(output_profile);
	}

	void IccTransform::setProfiles(const QString& input_profile, const QString& output_profile,
	                               const QString& proof_profile)
	{
		d->input_profile  = loadICCProfilFile(input_profile);
		d->output_profile = loadICCProfilFile(output_profile);
		d->proof_profile  = loadICCProfilFile(proof_profile);
	}

	void IccTransform::setProfiles(const QString& output_profile)
	{
		d->output_profile = loadICCProfilFile(output_profile);
	}

	void IccTransform::setProfiles(const QString& output_profile, const QString& proof_profile, bool forProof)
	{
		if (forProof)
		{
			d->output_profile = loadICCProfilFile(output_profile);
			d->proof_profile  = loadICCProfilFile(proof_profile);
		}
	}

	QString IccTransform::getEmbeddedProfileDescriptor()
	{
#ifdef ST_ICC_SUPPORT
		if (d->embedded_profile.isEmpty())
			return QString();

		cmsHPROFILE tmpProfile = cmsOpenProfileFromMem(d->embedded_profile.data(),
		                         (DWORD)d->embedded_profile.size());
		QString embeddedProfileDescriptor = QString(cmsTakeProductDesc(tmpProfile));
		cmsCloseProfile(tmpProfile);
		return embeddedProfileDescriptor;
#else
		return QString();
#endif
	}

	QString IccTransform::getInputProfileDescriptor()
	{
#ifdef ST_ICC_SUPPORT
		if (d->input_profile.isEmpty()) return QString();
		cmsHPROFILE tmpProfile = cmsOpenProfileFromMem(d->input_profile.data(), (DWORD)d->input_profile.size());
		QString embeddedProfileDescriptor = QString(cmsTakeProductDesc(tmpProfile));
		cmsCloseProfile(tmpProfile);
		return embeddedProfileDescriptor;
#else 
		return QString(); 
#endif 
	}

	QString IccTransform::getOutpoutProfileDescriptor()
	{
#ifdef ST_ICC_SUPPORT
		if (d->output_profile.isEmpty()) return QString();
		cmsHPROFILE tmpProfile = cmsOpenProfileFromMem(d->output_profile.data(), (DWORD)d->output_profile.size());
		QString embeddedProfileDescriptor = QString(cmsTakeProductDesc(tmpProfile));
		cmsCloseProfile(tmpProfile);
		return embeddedProfileDescriptor;
#else 
		return QString(); 
#endif 
	}

	QString IccTransform::getProofProfileDescriptor()
	{
#ifdef ST_ICC_SUPPORT
		if (d->proof_profile.isEmpty()) return QString();
		cmsHPROFILE tmpProfile = cmsOpenProfileFromMem(d->proof_profile.data(), (DWORD)d->proof_profile.size());
		QString embeddedProfileDescriptor = QString(cmsTakeProductDesc(tmpProfile));
		cmsCloseProfile(tmpProfile);
		return embeddedProfileDescriptor;
#else 
		return QString(); 
#endif 
	}

	bool IccTransform::apply(DImg& image)
	{
#ifdef ST_ICC_SUPPORT
		cmsHPROFILE   inprofile = 0, outprofile = 0, proofprofile = 0;
		cmsHTRANSFORM transform;
		int inputFormat = 0;
		int intent      = INTENT_PERCEPTUAL;

		switch (getRenderingIntent())
		{
			case 0:
				intent = INTENT_PERCEPTUAL;
				break;
			case 1:
				intent = INTENT_RELATIVE_COLORIMETRIC;
				break;
			case 2:
				intent = INTENT_SATURATION;
				break;
			case 3:
				intent = INTENT_ABSOLUTE_COLORIMETRIC;
				break;
		}

		//kDebug(50003) << "Intent is: " << intent << endl;

		if (d->has_embedded_profile)
		{
			inprofile = cmsOpenProfileFromMem(d->embedded_profile.data(),
			                                  (DWORD)d->embedded_profile.size());
		}
		else
		{
			inprofile = cmsOpenProfileFromMem(d->input_profile.data(),
			                                  (DWORD)d->input_profile.size());
		}
		if (inprofile == NULL)
		{
			qDebug() << "Error: Input profile is NULL" << endl;
			cmsCloseProfile(inprofile);
			return false;
		}

//     if (d->has_embedded_profile)
//     {
//         outprofile = cmsOpenProfileFromMem(d->embedded_profile.data(),
//                                            (DWORD)d->embedded_profile.size());
//     }
//     else
//     {
		outprofile = cmsOpenProfileFromMem(d->output_profile.data(),
		                                   (DWORD)d->output_profile.size());
//     }

		if (outprofile == NULL)
		{
			qDebug() << "Error: Output profile is NULL" << endl;
			cmsCloseProfile(outprofile);
			return false;
		}

		if (!d->do_proof_profile)
		{
			if (image.sixteenBit())
			{
				if (image.hasAlpha())
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAYA_16;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_16;
							break;
						default:
							inputFormat = TYPE_BGRA_16;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGRA_16,
					                               intent,
					                               cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAY_16;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_16;
							break;
						default:
							inputFormat = TYPE_BGR_16;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGR_16,
					                               intent,
					                               cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
			else
			{
				if (image.hasAlpha())
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAYA_8;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_8;
							break;
						default:
							inputFormat = TYPE_BGRA_8;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGRA_8,
					                               intent,
					                               cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAYA_8;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_8;
							//kDebug(50003) << "input profile: cmyk no alpha" << endl;
							break;
						default:
							inputFormat = TYPE_BGR_8;
							//kDebug(50003) << "input profile: default no alpha" << endl;
					}

					transform = cmsCreateTransform(inprofile, inputFormat, outprofile,
					                               TYPE_BGR_8, intent,
					                               cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
		}
		else
		{
			proofprofile = cmsOpenProfileFromMem(d->proof_profile.data(),
			                                     (DWORD)d->proof_profile.size());

			if (proofprofile == NULL)
			{
				qDebug() << "Error: Input profile is NULL" << endl;
				cmsCloseProfile(inprofile);
				cmsCloseProfile(outprofile);
				return false;
			}

			if (image.sixteenBit())
			{
				if (image.hasAlpha())
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGRA_16,
					                                       outprofile,
					                                       TYPE_BGRA_16,
					                                       proofprofile,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_16,
					                                       outprofile,
					                                       TYPE_BGR_16,
					                                       proofprofile,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
			else
			{
				if (image.hasAlpha())
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_8,
					                                       outprofile,
					                                       TYPE_BGR_8,
					                                       proofprofile,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_8,
					                                       outprofile,
					                                       TYPE_BGR_8,
					                                       proofprofile,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       INTENT_ABSOLUTE_COLORIMETRIC,
					                                       cmsFLAGS_WHITEBLACKCOMPENSATION);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
		}

		// We need to work using temp pixel buffer to apply ICC transformations.
		QVarLengthArray<uchar> transdata(image.bytesDepth());

		// Always working with uchar* prevent endianess problem.
		uchar *data = image.bits();

		// We scan all image pixels one by one.
		for (uint i = 0; i < image.width()*image.height()*image.bytesDepth(); i += image.bytesDepth())
		{
			// Apply ICC transformations.
			cmsDoTransform(transform, &data[i], &transdata[0], 1);

			// Copy buffer to source to update original image with ICC corrections.
			// Alpha channel is restored in all cases.
			memcpy(&data[i], &transdata[0], (image.bytesDepth() == 8) ? 6 : 3);
		}

		cmsDeleteTransform(transform);
		cmsCloseProfile(inprofile);
		cmsCloseProfile(outprofile);

		if (d->do_proof_profile)
			cmsCloseProfile(proofprofile);

		return true;
		#else 
		return false;
		#endif 
	}

	bool IccTransform::apply(DImg& image, QByteArray& profile, int intent, bool useBPC,
	                         bool checkGamut, bool useBuiltin)
	{
#ifdef ST_ICC_SUPPORT
		cmsHPROFILE   inprofile = 0, outprofile = 0, proofprofile = 0;
		cmsHTRANSFORM transform;
		int transformFlags = 0, inputFormat = 0;

		switch (intent)
		{
			case 0:
				intent = INTENT_PERCEPTUAL;
				break;
			case 1:
				intent = INTENT_RELATIVE_COLORIMETRIC;
				break;
			case 2:
				intent = INTENT_SATURATION;
				break;
			case 3:
				intent = INTENT_ABSOLUTE_COLORIMETRIC;
				break;
		}

		//kDebug(50003) << "Intent is: " << intent << endl;

		if (!profile.isNull())
		{
			inprofile = cmsOpenProfileFromMem(profile.data(),
			                                  (DWORD)profile.size());
		}
		else if (useBuiltin)
		{
			inprofile = cmsCreate_sRGBProfile();
		}
		else
		{
			inprofile = cmsOpenProfileFromMem(d->input_profile.data(),
			                                  (DWORD)d->input_profile.size());
		}

		if (inprofile == NULL)
		{
			qDebug() << "Error: Input profile is NULL" << endl;
			return false;
		}

		outprofile = cmsOpenProfileFromMem(d->output_profile.data(),
		                                   (DWORD)d->output_profile.size());

		if (outprofile == NULL)
		{
			qDebug() << "Error: Output profile is NULL" << endl;
			cmsCloseProfile(inprofile);
			return false;
		}

		if (useBPC)
		{
			transformFlags |= cmsFLAGS_WHITEBLACKCOMPENSATION;
		}

		if (!d->do_proof_profile)
		{
			if (image.sixteenBit())
			{
				if (image.hasAlpha())
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAYA_16;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_16;
							break;
						default:
							inputFormat = TYPE_BGRA_16;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGRA_16,
					                               intent,
					                               transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAY_16;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_16;
							break;
						default:
							inputFormat = TYPE_BGR_16;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGR_16,
					                               intent,
					                               transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
			else
			{
				if (image.hasAlpha())
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAYA_8;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_8;
							break;
						default:
							inputFormat = TYPE_BGRA_8;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGRA_8,
					                               intent,
					                               transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					switch (cmsGetColorSpace(inprofile))
					{
						case icSigGrayData:
							inputFormat = TYPE_GRAY_8;
							break;
						case icSigCmykData:
							inputFormat = TYPE_CMYK_8;
							break;
						default:
							inputFormat = TYPE_BGR_8;
					}

					transform = cmsCreateTransform(inprofile,
					                               inputFormat,
					                               outprofile,
					                               TYPE_BGR_8,
					                               intent,
					                               transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
		}
		else
		{
			proofprofile = cmsOpenProfileFromMem(d->proof_profile.data(),
			                                     (DWORD)d->proof_profile.size());

			if (proofprofile == NULL)
			{
				qDebug() << "Error: Input profile is NULL" << endl;
				cmsCloseProfile(inprofile);
				cmsCloseProfile(outprofile);
				return false;
			}

			transformFlags |= cmsFLAGS_SOFTPROOFING;
			if (checkGamut)
			{
				cmsSetAlarmCodes(126, 255, 255);
				transformFlags |= cmsFLAGS_GAMUTCHECK;
			}

			if (image.sixteenBit())
			{
				if (image.hasAlpha())
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGRA_16,
					                                       outprofile,
					                                       TYPE_BGRA_16,
					                                       proofprofile,
					                                       intent,
					                                       intent,
					                                       transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_16,
					                                       outprofile,
					                                       TYPE_BGR_16,
					                                       proofprofile,
					                                       intent,
					                                       intent,
					                                       transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
			else
			{
				if (image.hasAlpha())
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_8,
					                                       outprofile,
					                                       TYPE_BGR_8,
					                                       proofprofile,
					                                       intent,
					                                       intent,
					                                       transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
				else
				{
					transform = cmsCreateProofingTransform(inprofile,
					                                       TYPE_BGR_8,
					                                       outprofile,
					                                       TYPE_BGR_8,
					                                       proofprofile,
					                                       intent,
					                                       intent,
					                                       transformFlags);

					if (!transform)
					{
						qDebug() << "LCMS internal error: cannot create a color transform instance" << endl;
						return false;
					}
				}
			}
		}

		//kDebug(50003) << "Transform flags are: " << transformFlags << endl;

		// We need to work using temp pixel buffer to apply ICC transformations.
		QVarLengthArray<uchar>  transdata(image.bytesDepth());

		// Always working with uchar* prevent endianess problem.
		uchar *data = image.bits();

		// We scan all image pixels one by one.
		for (uint i = 0; i < image.width()*image.height()*image.bytesDepth(); i += image.bytesDepth())
		{
			// Apply ICC transformations.
			cmsDoTransform(transform, &data[i], &transdata[0], 1);

			// Copy buffer to source to update original image with ICC corrections.
			// Alpha channel is restored in all cases.
			memcpy(&data[i], &transdata[0], (image.bytesDepth() == 8) ? 6 : 3);
		}

		cmsDeleteTransform(transform);
		cmsCloseProfile(inprofile);
		cmsCloseProfile(outprofile);

		if (d->do_proof_profile)
			cmsCloseProfile(proofprofile);

#endif 
		return true;
	}

}  // namespace Digikam
