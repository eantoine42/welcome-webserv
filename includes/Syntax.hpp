/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Syntax.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lfrederi <lfrederi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 21:54:52 by lfrederi          #+#    #+#             */
/*   Updated: 2023/05/30 21:54:59 by lfrederi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SYNTAX_HPP
# define SYNTAX_HPP

#include <fstream>
#include <vector>
#include <map>
#include <ctime>

# define WHITESPACES " \n\r\t\f\v"


enum method_t {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	TOTAL_METHODS_INSTRUCTIONS
};

enum server_instruction_t {
	S_ROOT,
	LISTEN,
	SERVER_NAME,
	S_ERROR_PAGE,
	S_INDEX,
	S_AUTOINDEX,
	S_CLIENT_MAX_BODY_SIZE,
	S_CGI,
	LOCATION_INSTRUCTION,
	TOTAL_SERVER_INSTRUCTIONS
};

enum location_instruction_t {
	L_ROOT,
	METHODS,
	L_INDEX,
	L_CGI,
	L_AUTOINDEX,
	UPLOAD_DIR,
	RETURN,
	L_CLIENT_MAX_BODY_SIZE,
	L_ERROR_PAGE,
	URI,
	TOTAL_LOCATION_INSTRUCTIONS
};

enum request_header_name_t {
	ACCEPT_CHARSET,
	CONTENT_LENGTH,
	AUTHORIZATION,
	CONTENT_TYPE,
	DATE,
	HOST,
	REFERER,
	TRANSFER_ENCODING,
	USER_AGENT
};

enum answer_header_name_t {
	ALLOW,
	CONTENT_LANGUAGE,
	A_CONTENT_LENGTH,
	CONTENT_LOCATION,
	A_CONTENT_TYPE,
	A_DATE,
	LAST_MODIFIED,
	LOCATION,
	RETRY_AFTER,
	SERVER,
	A_TRANSFER_ENCODING,
	WWW_AUTHENTICATE
};

enum status_code_t {
	UNDEFINED,
	CONTINUE = 100,
	SWITCHING_PROTOCOLS,
	OK = 200,
	CREATED,
	ACCEPTED,
	NON_AUTHORITATIVE_INFORMATION,
	NO_CONTENT,
	RESET_CONTENT,
	PARTIAL_CONTENT,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY,
	FOUND,
	SEE_OTHER,
	NOT_MODIFIED,
	USE_PROXY,
	TEMPORARY_REDIRECT = 307,
	BAD_REQUEST = 400,
	UNAUTHORIZED,
	PAYMENT_REQUIRED,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	PROXY_AUTHENTICATION_REQUIRED,
	REQUEST_TIMEOUT,
	CONFLICT,
	GONE,
	LENGTH_REQUIRED,
	PRECONDITION_FAILED,
	PAYLOAD_TOO_LARGE,
	URI_TOO_LONG,
	UNSUPPORTED_MEDIA_TYPE,
	RANGE_NOT_SATISFIABLE,
	EXPECTATION_FAILED,
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED,
	BAD_GATEWAY,
	SERVICE_UNAVAILABLE,
	GATEWAY_TIMEOUT,
	HTTP_VERSION_NOT_SUPPORTED
};

enum mime_type_t {
	AUDIO_AAC,
	APPLICATION_X_ABIWORD,
	APPLICATION_OCTET_STREAM_ARC,
	VIDEO_X_MSVIDEO,
	APPLICATION_VND_AMAZON_EBOOK,
	APPLICATION_OCTET_STREAM,
	IMAGE_BMP,
	APPLICATION_X_BZIP,
	APPLICATION_X_BZIP2,
	APPLICATION_X_CSH,
	TEXT_CSS,
	TEXT_CSV,
	TEXT_PLAIN,
	APPLICATION_MSWORD,
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_WORDPROCESSINGML_DOCUMENT,
	APPLICATION_VND_MS_FONTOBJECT,
	APPLICATION_EPUB_ZIP,
	IMAGE_GIF,
	TEXT_HTM,
	TEXT_HTML,
	IMAGE_X_ICON,
	TEXT_CALENDAR,
	APPLICATION_JAVA_ARCHIVE,
	IMAGE_JPG,
	IMAGE_JPEG,
	APPLICATION_JAVASCRIPT,
	APPLICATION_JSON,
	AUDIO_MID,
	AUDIO_MIDI,
	VIDEO_MPEG,
	APPLICATION_VND_APPLE_INSTALLER_XML,
	APPLICATION_VND_OASIS_OPENDOCUMENT_PRESENTATION,
	APPLICATION_VND_OASIS_OPENDOCUMENT_SPREADSHEET,
	APPLICATION_VND_OASIS_OPENDOCUMENT_TEXT,
	AUDIO_OGG,
	VIDEO_OGG,
	APPLICATION_OGG,
	FONT_OTF,
	IMAGE_PNG,
	APPLICATION_PDF,
	APPLICATION_VND_MS_POWERPOINT,
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_PRESENTATIONML_PRESENTATION,
	APPLICATION_X_RAR_COMPRESSED,
	APPLICATION_RTF,
	APPLICATION_X_SH,
	IMAGE_SVG_XML,
	APPLICATION_X_SHOCKWAVE_FLASH,
	APPLICATION_X_TAR,
	IMAGE_TIF,
	IMAGE_TIFF,
	APPLICATION_TYPESCRIPT,
	FONT_TTF,
	APPLICATION_VND_VISIO,
	AUDIO_X_WAV,
	AUDIO_WEBM,
	VIDEO_WEBM,
	IMAGE_WEBP,
	FONT_WOFF,
	FONT_WOFF2,
	APPLICATION_XHTML_XML,
	APPLICATION_VND_MS_EXCEL,
	APPLICATION_VND_OPENXMLFORMATS_OFFICEDOCUMENT_SPREADSHEETML_SHEET,
	APPLICATION_XML,
	APPLICATION_VND_MOZILLA_XUL_XML,
	APPLICATION_ZIP,
	VIDEO_3GPP,
	VIDEO_3GPP2,
	APPLICATION_X_7Z_COMPRESSED,
	TOTAL_MIME_TYPES
};

class Syntax {
	public:
		~Syntax();
		struct method_tab_entry_t 
		{
			method_t		method_index;
			std::string		name;
		};

		struct server_instruction_tab_entry_t
		{
			server_instruction_t	instruction_index;
			std::string 	name;
		};
		struct location_instruction_tab_entry_t 
		{
			location_instruction_t	instruction_index;
			std::string 	name;
		};

		struct request_header_tab_entry_t {
			request_header_name_t	header_index;
			std::string		name;
		};

		struct answer_header_tab_entry_t {
			answer_header_name_t	header_index;
			std::string		name;
		};

		struct mime_type_entry_t {
			mime_type_t	mime_type_index;
			std::string ext;
			std::string	name;
		};

		static std::map<status_code_t, std::string> 		_response_status_map;
		static const method_tab_entry_t						method_tab[];
		static const server_instruction_tab_entry_t 		server_instructions_tab[];
		static const location_instruction_tab_entry_t 		location_instructions_tab[];
		static const request_header_tab_entry_t				request_header_tab[];
		static const answer_header_tab_entry_t				answer_header_tab[];
		static const mime_type_entry_t 						mime_types_tab[];

		static std::string				trimComments(const std::string &str);
		static std::string				trimWhitespaces(const std::string& str);
		static std::string				replaceConsecutiveSpaces(const std::string& str);
		static std::string				getLine(std::string str, size_t n);
		static size_t 					nbLines(std::string &str_config);
		static bool 					isNothing(std::string str, int pos = 0);
		static bool 					checkBrackets(std::string &config_string);
		static void 					testPath(const std::string &path);
		static std::string 				trimLineToI(std::string &str, size_t pos);
		static int						findClosingBracket(std::string str);
		static std::vector<std::string> splitString(std::string str, const std::string &charset = " ");
		static void 					formatConfFile(std::string &conf);
		static char 					checkChar(std::string str);
		static std::string 				intToString(int num);
		static int 						correctServerInstruction(std::vector<std::string> token);
		static int 						correctLocationInstruction(std::vector<std::string> token);
		static int 						correctMethodInstruction(std::vector<std::string> token);
		static void 					fill_response_status_map(std::map<status_code_t, std::string> &map);
		static std::string				getFormattedDate(std::time_t time);


	private:
		Syntax(); 
		Syntax(const Syntax& src);
		Syntax& operator=(const Syntax& rhs);
};
		
#endif
