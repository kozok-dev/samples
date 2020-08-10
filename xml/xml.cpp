#include <stdio.h>
#include <windows.h>
#include <objbase.h>
#include <msxml2.h>

class MyContentHandler : public ISAXContentHandler {
	HRESULT STDMETHODCALLTYPE characters(const wchar_t *pwchChars, int cchChars) {
		wprtn(pwchChars, cchChars);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE endDocument() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE startDocument() {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE endElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName) {
		printf("</");
		wprtn(pwchLocalName, cchLocalName);
		printf(">");
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE startElement(const wchar_t *pwchNamespaceUri, int cchNamespaceUri, const wchar_t *pwchLocalName, int cchLocalName, const wchar_t *pwchQName, int cchQName, ISAXAttributes *pAttributes) {
		const wchar_t *a;
		int i, j, k;

		printf("<");
		wprtn(pwchLocalName, cchLocalName);
		pAttributes->getLength(&j);
		for (i = 0; i < j; i++) {
			printf(" [");
			pAttributes->getLocalName(i, &a, &k);
			wprtn(a, k);
			printf(" = ");
			pAttributes->getValue(i, &a, &k);
			wprtn(a, k);
			printf("]");
		}
		printf(">");

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ignorableWhitespace(const wchar_t *pwchChars, int cchChars) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE endPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE startPrefixMapping(const wchar_t *pwchPrefix, int cchPrefix, const wchar_t *pwchUri, int cchUri) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE processingInstruction(const wchar_t *pwchTarget, int cchTarget, const wchar_t *pwchData, int cchData) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE putDocumentLocator(ISAXLocator *pLocator) {
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE skippedEntity(const wchar_t *pwchName, int cchName) {
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return 0;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) {
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE Release() {
		return 0;
	}

	void wprtn(const wchar_t *str, int len) {
		for (int i = 0; i < len; i++) putwchar(str[i]);
	}
};

class MyErrorHandler : public ISAXErrorHandler {
	HRESULT STDMETHODCALLTYPE error(ISAXLocator *pLocator, const wchar_t *pwchErrorMessage, HRESULT hrErrorCode) {
		wprintf(L"\nERROR:(%08x)%s", hrErrorCode, pwchErrorMessage);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE fatalError(ISAXLocator *pLocator, const wchar_t *pwchErrorMessage, HRESULT hrErrorCode) {
		wprintf(L"\nFATAL:(%08x)%s", hrErrorCode, pwchErrorMessage);
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ignorableWarning(ISAXLocator * pLocator, const wchar_t *pwchErrorMessage, HRESULT hrErrorCode) {
		wprintf(L"\nWARNING:(%08x)%s", hrErrorCode, pwchErrorMessage);
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		return 0;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void **ppvObject) {
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE Release() {
		return 0;
	}
};

void main()
{
	ISAXXMLReader *xmlrdr;
	MyContentHandler xmlch;
	MyErrorHandler xmleh;

	CoInitialize(NULL);
	if (SUCCEEDED(CoCreateInstance(CLSID_SAXXMLReader, NULL, CLSCTX_INPROC_SERVER, IID_ISAXXMLReader, (void **)&xmlrdr))) {
		xmlrdr->putContentHandler((ISAXContentHandler*)&xmlch);
		xmlrdr->putErrorHandler((ISAXErrorHandler*)&xmleh);
		xmlrdr->parseURL(L"xml.xml");
		xmlrdr->Release();
	}
	CoUninitialize();

	getchar();
}