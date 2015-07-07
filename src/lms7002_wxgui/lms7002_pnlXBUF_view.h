#ifndef __lms7002_pnlXBUF_view__
#define __lms7002_pnlXBUF_view__

/**
@file
Subclass of pnlXBUF_view, which is generated by wxFormBuilder.
*/

#include "lms7002_wxgui.h"

//// end generated include
#include <map>
#include "LMS7002M_parameters.h"
class LMS7002M;
/** Implementing pnlXBUF_view */
class lms7002_pnlXBUF_view : public pnlXBUF_view
{
	protected:
		// Handlers for pnlXBUF_view events.
		void ParameterChangeHandler( wxCommandEvent& event );
	public:
		/** Constructor */
		lms7002_pnlXBUF_view( wxWindow* parent );
	//// end generated class members
	lms7002_pnlXBUF_view(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
    void Initialize(LMS7002M* pControl);
    void UpdateGUI();
protected:
    LMS7002M* lmsControl;
	std::map<wxWindow*, LMS7Parameter> wndId2Enum;
};

#endif // __lms7002_pnlXBUF_view__
