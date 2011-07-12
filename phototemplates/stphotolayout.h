/****************************************************************************
**
** Copyright (C) 2006-2008 Starblitz. All rights reserved.
**
** This file is part of Starblitz Foto Suite.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file COPYING included in the packaging of
** this file.  
**
** Starblitz reserves all rights not expressly granted herein.
** 
** Strablitz (c) 2008
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef STPHOTOLAYOUT_H
#define STPHOTOLAYOUT_H

#include <QImage>
#include <QList>
#include <QDomNode>
#include <QFont>
#include <QColor>
#include <QDate>
#include "sterror.h"
#include "stphototemplatesexport.h"


/*!
	Template abstraction.
*/
class QDomDocument; 
class QDir;
class STFtpStatusWidget;
class STProgressIndicator;
class STErrorStack;
namespace STDom
{
	class STXmlPublisherSettings;
}
class ST_PHOTOTEMPLATES_EXPORT STPhotoLayoutTemplate
{

public: 
	ST_DECLARE_ERRORCLASS();

	//! Espai on col.locar una foto. 
	class ST_PHOTOTEMPLATES_EXPORT Frame : public QRectF
	{
	public:
		enum EnFrameType
		{
			TypePhoto, 
			TypeText, 
			TypeCalMonth, 
			TypeClipart,
			TypeRitchText
		};
	private:
		EnFrameType FrameType;
		QFont Font;
		QColor TextColor;
		QString Text;
		QColor BorderColor;
		int BorderSize;
		bool ShowPhotoId;
		bool ShowFileName;
		bool IgnoreModifyAll;
		bool IsTitle;
		qreal RotationAngle; 
		Qt::Alignment TextAlignment;
		int Month, Year; 
		QString ClipartFileName;
		QString MaskFileName;
		QString FrameFileName;
		QImage MaskImage;
		QImage FrameImage;
		int ZValue; 

	public:
		Frame(const QRectF& _Rect, double _RotationAngle = 0);
		void setFrameType(EnFrameType _Type) { FrameType = _Type;}
		EnFrameType frameType() const { return FrameType; }
		bool isTextFrame() const { return FrameType == TypeText || FrameType == TypeRitchText; }
		void setMonth(int _Value) { Month = _Value; }
		int month() const { return Month; }
		void setYear(int _Value) { Year = _Value; }
		int year() const { return Year; }
		void setFont(const QFont& _Font) { Font = _Font;}
		QFont font() const { return Font;}
		void setTextColor(const QColor& _TextColor) { TextColor = _TextColor; }
		QColor textColor() const { return TextColor; }
		void setRotationAngle(qreal _Angle) { RotationAngle = _Angle; } 
		qreal rotationAngle() const { return RotationAngle; }
		void setTextAlignment(Qt::Alignment _Value) { TextAlignment = _Value; }
		Qt::Alignment textAlignment() const { return TextAlignment; }
		void setText(const QString& _Text);
		QString text() const;
		void setBorderColor(const QColor& _BorderColor) { BorderColor = _BorderColor; }
		QColor borderColor() const { return BorderColor; }
		void setBorderSize(int _BorderSize) { BorderSize = _BorderSize; }
		int borderSize() const { return BorderSize; }
		void setShowPhotoId( bool _Value) { ShowPhotoId = _Value; }
		bool showPhotoId() const { return ShowPhotoId; }
		void setShowFileName( bool _Value) { ShowFileName = _Value; }
		bool showFileName() const { return ShowFileName; }
		void setIgnoreModifyAll(bool _Value) { IgnoreModifyAll = _Value; }
		bool ignoreModifyAll() const { return IgnoreModifyAll; }
		void setClipartFileName(const QString& _Value) { ClipartFileName = _Value; }
		//! \return Clipart file name relative to templates path.
		QString clipartFileName() const { return ClipartFileName; }
		void setIsTitle(bool _Value) { IsTitle = _Value; }
		bool isTitle() const { return IsTitle; }

		// ---- Masks ----
		void setMaskFileName(const QString& _Value) { MaskFileName = _Value; }
		void setMask(const QString& _FilenameToSave, const QImage& _MaskImage); 
		//! \return Clipart file name relative to templates path.
		QString maskFileName() const { return MaskFileName; }
		QImage maskImage() const;
		//! \return MaskImage or loads it from disk if is null
		QImage maskImage(const QDir& _TemplateDir) const;
		bool hasToSaveMask(const QString& _DestFileName) const;

		// ---- Frames ----
		void setFrameFileName(const QString& _Value) { FrameFileName = _Value; }
		void setFrame(const QString& _FilenameToSave, const QImage& _FrameImage);
		QString frameFileName() const { return FrameFileName; }
		QImage frameImage() const;
		//! \return MaskImage or loads it from disk if is null
		QImage frameImage(const QDir& _TemplateDir) const;
		bool hasToSaveFrame(const QString& _DestFileName) const;


		void setZValue(int _Value) { ZValue = _Value; }
		int zValue() const { return ZValue; }
	};

	enum EnType
	{
		TCard, 
		TAlbum,
		TIndex, 
		TCDCover,
		TBusinesCard,
		TBookMark, 
		TDeco
	};
	typedef QList<Frame> TFrameList;
	typedef QList<QImage> TImageList;
	typedef QList<QDate> TDateList;

private:
	QSizeF Size;
	QSizeF BaseSize;
	int Dpi; 
	QString Description; 
	QString TypeName;
	QString SubTypeName;
	TFrameList FrameList;
	QString BackgroundImageFile;
	QString SuperImposeImageFile;
	QString DSIImageFile;
	QString ThumbnailImageFile;
	QImage SuperImposeImage; 
	QImage DSIImage; 
	QImage BackgroundImage;
	QImage ThumbnailImage;
	QString TemplateFilePath;
	int NumPhotoFrames;
	int NumMonthFrames;
	bool IsFirstPage;
	QColor BGColor;
	bool ModifyAllFrames;
	bool RenderFramesOnly;
	QString CurrLocale;
	TDateList Holidays; 
	QStringList HolidayFuncs; 
	int Key;
	bool Encrypted;
	static int CurrKeyValue;
	Qt::AspectRatioMode AspectRatioMode;

	void addPicGrid(const Frame& _ParentFrame, const QDomElement& _Frame);
	void addFrame(const Frame& _ParentFrame, const QDomElement& _Frame);
	void loadLocaleImage(const QString& _Locale, const QDomElement& _Element, QImage& _Image);
	//! Obté el nom del fitxer de l'element _Element segons el locale. 
	void setLocaleImageFile(const QString& _Locale, const QDomElement& _Element, QString& _ImageFileName);
	void loadImageIfNull(const QString& _FileName, QImage& _Image) const;
	QImage superimposeImage();
	QImage dsiImage();
	QImage backgroundImage();	
	void getNewKey(); 

public:
	STPhotoLayoutTemplate(const QString& _Locale = "");
	void makePathsRelative(const QDir& _RootDir);
	void storeCliparts(const QDir& _ClipartSharedDir, const QDir& _StorageDir);
	void storeBackgrounds(const QDir& _StorageDir);
	void storeMasks(const QDir& _StorageDir);
	void storeFrames(const QDir& _StorageDir);
	QDomElement createElement(QDomDocument& _Doc);
	//! Es configura a partir de la informació d'un node del tipus: <layouttemplate>
	void load(QDomNode& _Node);
	//! Sobrecàrrega de l'anterior, però indicant les mides.
	void load(QDomNode& _Node, QSizeF& _Size, int _Dpi);
	int biggestFrameIndex();
	void addSubTittleTextFrame(int _FrameIndex);
	void setText(int _FrameIndex, const QString& _Text);
	void splitXFrame(int _FrameIndex);
	void splitYFrame(int _FrameIndex);
	//! Afegeix un frame a la llista de frames.
	void addFrame(const Frame& _Frame);
	//! Retorna la resolució en DPis.
	int dpi() const;
	bool modifyAllFrames() const { return ModifyAllFrames; }
	void setModifyAllFrames(bool _Value) { ModifyAllFrames = _Value; }
	bool encrypted() const { return Encrypted; }
	void setEncrypted(bool _Value) { Encrypted = _Value; }
	
	bool renderFramesOnly() const { return RenderFramesOnly; }
	void setRenderFramesOnly( bool _Value ) { RenderFramesOnly = _Value; }
	
	//! Base Size for template. Base Size is size used for autogenerated multi templates like businescard templates.
	QSizeF baseSize() const;
	//! Assigna la resolució en Dpis.
	void setDpi(int _Dpi);
	//! Nom de tipus del template.
	QString typeName() const;
	void setTypeName(const QString& _TypeName);
	//! Nom de sub-tipus del template.
	QString subTypeName() const;
	void setSubTypeName(const QString& _SubTypeName);
	//! Descripció del template
	QString description() const;
	//! Assigna la descripció del template
	void setDescription(const QString& _Description);
	//! Retorna la mida en pixels de la imatge a partir de les DPI i de la mida en mm de la frame.
	QSize pixelSize() const;
	static QSize pixelSize(const QSizeF& _Size, int _Dpi);
	//! Retorna l'altura que hauria de fer segons les proporcions del template una imatge amb amplada _Width.
	int heightForWidth(int _Width) const;
	int bigSide() const;
	int smallSide() const;
	//! Calcula una mida propocional al template que cap dins _RefSize.
	QSize fitInSize(const QSize& _RefSize) const;
	//! Retorna la mida en mm de la imatge.
	QSizeF size() const;
	//! Assigna la mida en mm de la imatge.
	void setSize(const QSizeF& _Size);
	//! Converts all frames proportional to new size _Size; 
	void convertToSize(const QSizeF& _Size); 
	//! Retorna l'index d'una frame donat una posició dins la mida _Size;
	int frameAtIndex(const QPointF& _Posicion, const QSize& _ImageSize);
	//! Retorna una rect amb les mides de _Rect relatives a _DestSize
	QRect translateRect(const QRectF& _Rect, const QSize& _DestSize) const;
	QRectF translateRectf(const QRectF& _Rect, const QSizeF& _DestSize) const;
	//! Retorna una imatge de la mida _ThumbnailSize amb recuadres on representa que hi han d'anar les fotos.
	QImage sampleThumbnail(const QSize& _ThumbnailSize, bool _LayoutOrientation = true) ;
	//! Path del fitxer on carreguem el template.
	void setTemplateFilePath(const QString& _TemplateFilePath);
	QString templateFilePath() const ;
	//! Retorna una llista de les frames.
	TFrameList frames() const;
	TFrameList monthFrames() const;
	//! Conversió de _Type a String;
	static QString typeToStr(EnType _Type);
	//! Path of superimpose Image.
	QString superImposeImageFilePath() const;
	//! Path of DSI Image.
	QString dsiImageFilePath() const;
	QString backgroundImageAbsoluteFilePath() const;
	//! Path de la imatge de background.
	QString backgroundImageFilePath() const;
	QString backgroundImageFile() const { return BackgroundImageFile; }
	void setBackgroundImageFile(const QString& _FileName);
	int key() const { return Key; }
	void setKey(int _Value) { Key = _Value; }
	//! It depends on template Key.(Always diferent if the key is never assigned).
	bool operator!=(const STPhotoLayoutTemplate& _Other) const;
	bool operator==(const STPhotoLayoutTemplate& _Other) const;
	//! Te en compte el nombre de frames un template amb + Frames > Template amb - frames.
	bool operator<(const STPhotoLayoutTemplate& _Other) const;
	bool operator>(const STPhotoLayoutTemplate& _Other) const;
	//! El valor _Value indica el nombre de Frames.
	bool operator<(int _Value) const;
	bool operator>(int _Value) const;
	//! Numero de frames de foto (No de texte).
	int numPhotoFrames() const;
	int numMonthFrames() const;
	void clearMonthFrames();
	//! Sets _Year to all year text frames
	void setYear(int _Year);
	//! És la primer pàgina ?
	bool isFirstPage() const;
	void setIsFirstPage(bool _Value) { IsFirstPage = _Value; }
	//! Color de fons (Per defecte és blanc.)
	QColor backgroundcolor() const;
	void setBackgroundColor(const QColor& _Color) { BGColor = _Color; }
	void setHolidays(const TDateList& _Holidays) { Holidays = _Holidays; }
	TDateList holidays(int _Year) const;
	void setHolidayFuncs(const QStringList& _HolidayFuncs) { HolidayFuncs = _HolidayFuncs; }
	//! Retorna el fitxer de thumbNail corresponent a _ImgFile
	static QString thumbnailImage(const QString& _ImgFile);
	QImage thumbnailImage();
	QImage thumbnailImage() const;
	bool hasSameFrames(const STPhotoLayoutTemplate& _Other) const;
	bool hasTextFrames() const;
	bool hasClipartFrames() const;
	int titleFrameIndex() const;

	Qt::AspectRatioMode aspectRatioMode() const { return AspectRatioMode; }
	void setAspectRatioMode(Qt::AspectRatioMode _Value) { AspectRatioMode = _Value; }

	bool resourcesOnDisk() const;
	void downloadResources(const STDom::STXmlPublisherSettings& _PublisherSettings, STFtpStatusWidget* _StatusWidget);

	void generateKey();
};

bool operator<(int _Value, const STPhotoLayoutTemplate& _Other);

/**
Classe que maneja fitxers .kpspltml que son plantilles que indiquen distribucions de imatges dins la foto.
Els fitxers .kpspltml (Kiosk Photo Server Photo Layout Template Markup Language) tenen la següent estructura: 
<!DOCTYPE KPSPLTML>
<KPSPLTML revision="##.#.#">
	<author name="Shadow" organization="Starblitz" email="shadow@starblitz-k.com"/>
	<!-- Llistat de subtipus de cada tipus amb la descripció i la icona. -->
	<subtypes type="typename">
		<subtype name="subtypename">
			<description ca="Divertit" es="Divertido" en="Funny"/>
			<icon src="./funsubtype.png" />
			<resourcesdir src="./" /> <!-- Indica on estan els directoris mask i clipart de les máscares i els cliparts -->
		</subtype>
	<subtypes>
	<templatelist type="card|album|index|postcard|cdcover|businescard|bookmark|deco" subtype="">
		<!-- card : Fotos de carnet. -->
		<!-- album : Templates de pàgina d'àlbum de foto. -->
		<!-- index : Foto index. -->
		<!-- cdcover : Caràtula de CDs. -->
		<!-- businescard : Targeta de presentació. -->
		<!-- bookmark : Punt de llibre. -->
		<!-- deco : Decorativa. -->
		
		<!-- subtype.(Opcional) referent a name de l'atribut subtype de subtypes. -->
		<layouttemplate with="###.## -amplada en mm-" height="###.## -altura en mm-" dpi="### -punts per pulzada-" backgroundcolor="#000000" firstpage="true" modifyallframes="true" renderframesonly="true" aspectratiomode="0">
			<!-- firstpage : Indica si és la primera pàgina de l'àlbum. -->
			<!-- modifyallframes : Indica si totes les frames es modificaran alhora -->
			<!-- renderframesonly : Només renderitza frame per frame tallada a mida -->
			<!-- aspectratiomode: See Qt::AspectRatioMode -->
			<!-- Parametres requerits: with, height, dpi -->
			<description ca="-Descripció del format-" es="" en="" .../><!-- Es buscarà el locale actual -->
			<!-- Imatge de fons, S'aplica la primera de tots abans que les frames -->
			<backgroundimage src_all="./TEMPLATES/Fun/wanted.png" src_ca="" src_en=""  />
			<!-- Imatge per a sobreposar -->
			<!-- S'adapta a la mida del seu frame pare. -->
			<superimposeimage src_all="./TEMPLATES/Fun/wanted.png" src_ca="" src_en=""  />
			
			<!-- dsiimage -> Dummy superimpose image. Imatge que se sobreposa a cada frame. Util per fer de guia. -->
			<dsiimage src_all="./TEMPLATES/Fun/dummy.png" src_ca="" src_en=""  />

			<!-- Frame de texte, permet indicar l'estil de la font. -->
			<textframe width="10" height="10" vpos="center" hpos="center" fontfamily="Arial" fontsize="10" shadow="true" textalignment="center" textvertical="true"/>

			<!-- Frame per a ajustar imatge -->
			<frame with="###.## -amplada en mm-" height="###.## -altura en mm-" vpos="center|top|bottom|###.##" hpos="center|left|right|###.##" rotation="### --Rotació en graus--" showphotoid="true" ignoremodifyall="true"
				masksrc="" framesrc="">
				<!-- Parametres requerits: with, height -->
				<!-- vpos -> indica la posició vertical del layout dins el template. Es poden indicar pocions relatives: center, top, bottom o una mida en mm referent a la part superior de la frame pare.-
				Per defecte: center -->
				<!-- hpos -> indica la posició vertical del layout dins el template. Es poden indicar pocions relatives: center, left, right o una mida en mm referent a la part esquerre de la frame pare.
				Per defecte: center -->
				<!--ignoremodifyall="true" Indica que no li afectarà l'atribut modifyallframes -->
				<frame>
				<!-- Les mides relatives seràn a partir de la frame pare! -->
				</frame>
				<picgrid cellwidth="###.## -amplada de cada casella en mm-" cellheight="###.## -altura de cada casella en mm-" halign="center|left|right" valign="center|top|bottom"/>
				<!-- picgrid generarà una sèrie de frames filles, tantes com n'hi càpiguen.>
				<!-- Parametres requerits: cellwidth, celheight -->
				<!-- lalign i valign indiquen on s'alineara la grid de fotos en cas de que no encaixin completament a la frame. 
				Per defecte: center, center. -->
			</frame>
		</layouttemplate>
		<layouttemplate>
			(...)
		</layouttemplate>
		(...)
 	</templatelist>
	<templatelist type="...">
		(...)
	</templatelist>
	(...)
<KPSPLTML revision="" >
 

@author shadow
*/

class ST_PHOTOTEMPLATES_EXPORT STPhotoLayoutSubtype
{
	QString Name;
	QString Description; 
	QString IconFile;
	QString MainType;
	QString ToolTipText; 
	QString ResourcesDir;

public:
	STPhotoLayoutSubtype(const QString& _Name, const QString& _Description, 
		const QString& _IconFile, const QString& _MainType, const QString& _ToolTipText, const QString& _ResourcesDir) 
		: Name(_Name), Description(_Description), IconFile(_IconFile), MainType(_MainType), ToolTipText(_ToolTipText), ResourcesDir(_ResourcesDir) {}
	QString name() const  { return Name; }
	QString description() const { return Description; }
	QString iconFile() const { return IconFile; }
	QString mainType() const { return MainType; }
	QString toolTipText() const { return ToolTipText; }
	QString resourcesDir() const { return ResourcesDir; }
};

class ST_PHOTOTEMPLATES_EXPORT STPhotoBookTemplate
{

public:
	enum EnPreprocessType
	{
		TypeNone = 0,
		TypeBooklet = 1
	};

	ST_DECLARE_ERRORCLASS();
	typedef QList<STPhotoLayoutTemplate> TTemplateList;
	typedef QList<QColor> TColorList;
	typedef QList<QRectF> TMarginRectList; 
	static QString DefaultRef;

private:
	TTemplateList Templates;
	TColorList Colors;
	QString Description;
	QString TemplatesDirPath;
	QString ClipartDirPath; 
	QString MaskDirPath; 
	QString FrameDirPath;
	QString AuthorName; 
	QString AuthorOrganization; 
	QString AuthorEmail;
	QSizeF Size;
	int Dpis;
	int MaxPages;
	int MinPages;
	int ModPages;
	int NumOptimalImagesPerPage;
	double PrintPageWidth;
	QString CurrLocale; 
	TMarginRectList PageMarginRects; 
	TMarginRectList CoverMarginRects; 
	bool CutPagesOnPrint; 
	EnPreprocessType PreprocessType;
	QString Ref; //Template Reference
	bool IsAtomic;
	bool PreferMinPages;
	bool IsVariableCalendar;
	bool PrintFirstPageAtLast;
	bool AutoGenerateTemplates;
	QString InfoUrl;

public:
	STPhotoBookTemplate(const QString& _Locale = "");
	TTemplateList& templates();
	TTemplateList templates() const;
	void removeTextTemplates();
	void setTitleText(const QString& _Text);
	void setColors(const TColorList& _Colors);
	TColorList colors() const;
	void setDescription(const QString& _Value);
	QString description() const;
	void setBGImageDirPath(const QString& _Value);
	QString bgImageDirPath() const;
	void setClipartDirPath(const QString& _Value);
	QString clipartDirPath() const;
	void setMaskDirPath(const QString& _Value);
	QString maskDirPath() const;
	void setFrameDirPath(const QString& _Value);
	QString frameDirPath() const;
	void setSize(const QSizeF& _Size);
	QSizeF size() const;
	void setDpis(int _Dpis);
	int dpis() const;
	int dpM() const;
	void setMaxPages(int _Value ) { MaxPages = _Value; }
	int maxPages() const { return MaxPages; }
	void setMinPages(int _Value ) { MinPages = _Value; }
	int minPages() const { return MinPages; }
	void setModPages(int _Value) { ModPages = _Value; }
	void setAutoGenerateTemplates(bool _Value) { AutoGenerateTemplates = _Value; }
	bool autoGenerateTemplates() const { return AutoGenerateTemplates; }
	void setNumOptimalImagesPerPage(int _Value) { NumOptimalImagesPerPage = _Value; }
	int numOptimalImagesPerPage() const { return NumOptimalImagesPerPage; }

	void setPreferMinPages(bool _Value) { PreferMinPages = _Value; }
	bool preferMinPages() const { return PreferMinPages; }
	void setIsVariableCalendar(bool _Value) { IsVariableCalendar = _Value; }
	bool isVariableCalendar() const { return  IsVariableCalendar; }
	//! The number of pages mod modPages() must be 0.
	int modPages() const { return ModPages; }
	void setPrintPageWidth(double _Value) { PrintPageWidth = _Value; }
	double printPageWidth() const { return PrintPageWidth; }
	int numPages() const { return size().width() / PrintPageWidth; }
	void clear();
	void clearMonthFrames();
	void loadAlbum(const QDomNode& _AlbumNode, const QString& _TemplateFilePath, const STPhotoLayoutTemplate::TDateList& _Holidays = STPhotoLayoutTemplate::TDateList(), const QStringList& _HolidayFuncs = QStringList()); 
	void load(const QString& _TemplateFileName);
	void downloadRemoteContents(const STDom::STXmlPublisherSettings& _PubSettings, STProgressIndicator* _ProgressIndicator, STFtpStatusWidget* _StatusWidget, STErrorStack* _ErrorStack = 0);
	void save(const QString& _TemplateFileName); 
	QString authorName() const { return AuthorName; }
	void setAuthorName(const QString& _Value) { AuthorName = _Value; }
	QString authorOrganization() const { return AuthorOrganization; }
	void setAuthorOrganization(const QString& _Value) { AuthorOrganization = _Value; }
	QString authorEmail() const { return AuthorEmail; }
	void setAuthorEmail(const QString& _Value) { AuthorEmail = _Value; }
	TMarginRectList pageMarginRects() const { return PageMarginRects; }
	void setPageMarginRects(const TMarginRectList& _MarginRects) { PageMarginRects = _MarginRects; }
	TMarginRectList coverMarginRects() const { return CoverMarginRects; }
	void setCoverMarginRects(const TMarginRectList& _MarginRects) { CoverMarginRects = _MarginRects; }
	bool cutPagesOnPrint() { return CutPagesOnPrint; }
	void setCutPagesOnPrint(bool _Value) { CutPagesOnPrint = _Value; }
	bool printFirstPageAtLast() const { return PrintFirstPageAtLast; }
	void setPrintFirstPageAtLast(bool _Value) { PrintFirstPageAtLast = _Value; }
	EnPreprocessType preprocessType() const { return PreprocessType; }
	void setPreprocessType(EnPreprocessType _Value) { PreprocessType = _Value; }
	//! Template reference.
	QString ref() const { return Ref; }
	void setRef(const QString& _Value) { Ref = _Value; }
	bool isAtomic() const { return IsAtomic; }
	void setIsAtomic(bool _Value) { IsAtomic = _Value; }
	QString infoUrl() const { return InfoUrl; }
	void setInfoUrl(const QString& _Value) { InfoUrl = _Value; }
	bool hasFirstPages() const;
};


class ST_PHOTOTEMPLATES_EXPORT STPhotoLayout
{

public: 	
	enum EnLayoutType
	{
		TypePhotoBook,
		TypeCalendar,
		TypeCard,
		TypeIdPhoto,
		TypeMultiPhoto
	};

	enum EnResourceSubDirType
	{
		MasksDir,
		FramesDir,
		ClipartDir,
		BackgroundsDir
	};

	ST_DECLARE_ERRORCLASS();
	typedef QList<STPhotoLayoutTemplate> TTemplateList;
	typedef QList<STPhotoLayoutSubtype> TSubtypeList;
	typedef QList<STPhotoBookTemplate> TAlbumsList; //FIXME: Deprecated, Remove it.

private:
	QString CurrLocale; 
	TAlbumsList Albums;//FIXME: Remove it 
	TTemplateList Templates;
	TSubtypeList SubTypes;
	void loadFile(const QString& _XmlTemplateFile, const QString& _Type = "");
	void loadTemplateList(const QDomNode& _TemplateListNode, const QString& _TemplateFilePath, const QString& _Type, const STPhotoLayoutTemplate::TDateList& _Holidays, const QStringList& _HolidayFuncs);
	void loadAlbum(const QDomNode& _AlbumNode, const QString& _TemplateFilePath, const STPhotoLayoutTemplate::TDateList& _Holidays, const QStringList& _HolidayFuncs);
	void addSubTypes(const QDomElement& _SubTypeListElement, const QString& _MainType, const QString& _PathRoot);

public:
	STPhotoLayout(const QString& _Locale = "");
	void setLocale(const QString& _Locale) { CurrLocale = _Locale; }  
	//! Esborra la llista de templates.
	void clear();
	void saveTemplateList(const QString& _XmlTemplateFile, const QString& _Author = "", const QString& _Organization = "", const QString& 	_Email = "");
	//! Carrega el fitxer _XmlTemplateFile i l'afegeix a la llista de templates.
	void load(const QString& _XmlTemplateFile);
	//! Carrega els templates del tipus _Type el fitxer _XmlTemplateFile i l'afegeix a la llista de templates.
	void load(const QString& _XmlTemplateFile, STPhotoLayoutTemplate::EnType _Type);
	//! Carrega de tots els fitxers del directori _Dir
	void loadDir(const QString& _Dir);
	//! Carrega de tots els fitxers del directori _Dir i de un tipus específic.
	void loadDir(const QString& _Dir, STPhotoLayoutTemplate::EnType _Type);
	void addTemplate(const STPhotoLayoutTemplate& _Template);
	//! Retorna una llista de tots els templates.
	TTemplateList templates() const;
	//! \return a list of all loaded templates having subtype name == _Name
	TTemplateList templates(const QString& _SubTypeName) const;
	void setTemplates(const TTemplateList& _Templates) { Templates = _Templates; }
	//! Retorna tots els àlbums.
	TAlbumsList albums() const; //FIXME: Deprecated
	//! Llista de subtipus per als templates del tipus que hem carregat.
	TSubtypeList subTypes() const;
	static void loadHolidays(const QDomElement& _DocEl, STPhotoLayoutTemplate::TDateList& _LHoliDays, QStringList& _LHolidDayFuncs);
	static QString resourceSubDirName(EnResourceSubDirType _Type); 
};

#endif
