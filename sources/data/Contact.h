#ifndef CONTACT_H
#define CONTACT_H

#include <string>

using namespace std;

class Contact
{
public:
    std::string EmailId;

    std::string AlternateEmailIds;
    std::string FirstName;
    std::string MiddleName;
    std::string LastName;
    std::string WebSite;

    std::string AddressLine1;
    std::string AddressLine2;
    std::string City;
    std::string State;
    std::string Country;
    std::string ZipCode;

    std::string WorkPhone;
    std::string HomePhone;
    std::string Fax;
    std::string MobileNo;
    std::string NickName;

    std::string Anniversary;
    std::string Notes;
    std::string Photo;

	Contact()
	{
		EmailId = "-";
		AlternateEmailIds = "-";

		FirstName = "-";
		MiddleName = "-";
		LastName = "-";
		WebSite = "-";

		AddressLine1 = "-";
		AddressLine2 = "-";
		City = "-";
		State = "-";
		Country = "-";
		ZipCode = "-";

		WorkPhone = "-";
		HomePhone = "-";
		Fax = "-";
		MobileNo = "-";
		NickName = "-";

		Anniversary = "-";
		Notes = "-";
		Photo = "-";
	}
};

#endif
