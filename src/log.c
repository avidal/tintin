/******************************************************************************
*   TinTin++                                                                  *
*   Copyright (C) 2004 (See CREDITS file)                                     *
*                                                                             *
*   This program is protected under the GNU GPL (See COPYING)                 *
*                                                                             *
*   This program is free software; you can redistribute it and/or modify      *
*   it under the terms of the GNU General Public License as published by      *
*   the Free Software Foundation; either version 2 of the License, or         *
*   (at your option) any later version.                                       *
*                                                                             *
*   This program is distributed in the hope that it will be useful,           *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
*   GNU General Public License for more details.                              *
*                                                                             *
*   You should have received a copy of the GNU General Public License         *
*   along with this program; if not, write to the Free Software               *
*   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
*******************************************************************************/

/******************************************************************************
*                (T)he K(I)cki(N) (T)ickin D(I)kumud Clie(N)t                 *
*                                                                             *
*                      coded by Igor van den Hoven 2004                       *
******************************************************************************/

#include "tintin.h"


void logit(struct session *ses, char *txt, FILE *file, int newline)
{
	char out[BUFFER_SIZE];

	if (HAS_BIT(ses->flags, SES_FLAG_LOGPLAIN))
	{
		strip_vt102_codes(txt, out);
	}
	else if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
	{
		vt102_to_html(ses, txt, out);
	}
	else
	{
		strcpy(out, txt);
	}

	if (newline)
	{
		strcat(out, "\n");
	}
	fputs(out, file);

	fflush(file);
}


DO_COMMAND(do_log)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  FALSE);
	substitute(ses, left, left, SUB_VAR|SUB_FUN);

	arg = get_arg_in_braces(arg, right, TRUE);
	substitute(ses, right, right, SUB_VAR|SUB_FUN);

	if (ses->logfile)
	{
		fclose(ses->logfile);
		ses->logfile = NULL;
		show_message(ses, -1, "#OK: LOGGING TURNED OFF.");
	}
	else if (*left == 0 || *right == 0 || (!is_abbrev(left, "APPEND") && !is_abbrev(left, "OVERWRITE")))
	{
		tintin_printf(ses, "#SYNTAX: #LOG [<APPEND|OVERWRITE> <FILENAME>]");
	}
	else
	{
		if (is_abbrev(left, "APPEND"))
		{
			if ((ses->logfile = fopen(right, "a")))
			{
				fseek(ses->logfile, 0, SEEK_END);

				if (ftell(ses->logfile) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					write_html_header(ses->logfile);
				}
				show_message(ses, -1, "#OK: LOGGING OUTPUT TO '%s' FILESIZE: %ld", right, ftell(ses->logfile));
			}
			else
			{
				tintin_printf2(ses, "#ERROR: #LOG {%s} {%s} - COULDN'T OPEN FILE.", left, right);
			}
		}
		else
		{
			if ((ses->logfile = fopen(right, "w")))
			{
				if (HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
				{
					write_html_header(ses->logfile);
				}
				show_message(ses, -1, "#OK: LOGGING OUTPUT TO '%s'", right);
			}
			else
			{
				tintin_printf2(ses, "#ERROR: #LOG {%s} {%s} - COULDN'T OPEN FILE.", left, right);
			}
		}
	}
	return ses;
}


DO_COMMAND(do_logline)
{
	char left[BUFFER_SIZE], right[BUFFER_SIZE];

	arg = get_arg_in_braces(arg, left,  0);
	arg = get_arg_in_braces(arg, right, 1);

	tintin_printf2(ses, "#LOGLINE: Old command, use #LINE LOG instead.");

	substitute(ses, right, right, SUB_ESC|SUB_COL);

	if (ses->logline)
	{
		return ses;
	}

	if ((ses->logline = fopen(left, "a")))
	{
		fseek(ses->logline, 0, SEEK_END);

		if (ftell(ses->logline) == 0 && HAS_BIT(ses->flags, SES_FLAG_LOGHTML))
		{
			write_html_header(ses->logline);
		}

		if (*right)
		{
			logit(ses, right, ses->logline, TRUE);

			fclose(ses->logline);
			ses->logline = NULL;
		}
	}
	else
	{
		tintin_printf(ses, "#ERROR: #LOGLINE {%s} - COULDN'T OPEN FILE.", left);
	}
	return ses;
}

void write_html_header(FILE *fp)
{
	char *header =
		"<html>\n"
		"<head>\n"
		"<meta http-equiv='content-type' content='text/html; charset=iso-8859-1'>\n"
		"<meta name='description' content='Generated by TinTin++ "VERSION_NUM" - http://tintin.sourceforge.net'>\n"
		"<style type='text/css'>\n"
		"{\n\tfont-family: Courier;\n\tfont-size: 10pt;\n}\n"
		".d30{ color: #000; } .l30{ color: #555; } .b40{ background-color: #000; } .b50{ background-color: #555 }\n"
		".d31{ color: #B00; } .l31{ color: #F55; } .b41{ background-color: #B00; } .b51{ background-color: #F55 }\n"
		".d32{ color: #0B0; } .l32{ color: #5F5; } .b42{ background-color: #0B0; } .b52{ background-color: #5F5 }\n"
		".d33{ color: #BB0; } .l33{ color: #FF5; } .b43{ background-color: #BB0; } .b53{ background-color: #FF5 }\n"
		".d34{ color: #00B; } .l34{ color: #55F; } .b44{ background-color: #00B; } .b54{ background-color: #55F }\n"
		".d35{ color: #B0B; } .l35{ color: #F5F; } .b45{ background-color: #B0B; } .b55{ background-color: #F5F }\n"
		".d36{ color: #0BB; } .l36{ color: #5FF; } .b46{ background-color: #0BB; } .b56{ background-color: #5FF }\n"
		".d37{ color: #BBB; } .l37{ color: #FFF; } .b47{ background-color: #BBB; } .b57{ background-color: #FFF }\n"
		".d38{ color: #FFF; } .l38{ color: #FFF; } .b48{ background-color: #000; } .b58{ background-color: #000 }\n"
		".d39{ color: #FFF; } .l39{ color: #FFF; } .b49{ background-color: #000; } .b59{ background-color: #000 }\n"
		"</style>\n"
		"<body bgcolor='#000000'>\n"
		"</head>\n"
		"<pre>\n"
		"<span class='b49'><span class='d39'>\n";
	fputs(header, fp);
}


void vt102_to_html(struct session *ses, char *txt, char *out)
{
	char tmp[BUFFER_SIZE], *pti, *pto, x[] = { '0', '5', '8', 'B', 'D', 'F' };
	int vtc, fgc, bgc, cnt;

	vtc = ses->vtc;
	fgc = ses->fgc;
	bgc = ses->bgc;

	pti = txt;
	pto = out;

	while (*pti)
	{
		while (skip_vt102_codes_non_graph(pti))
		{
			pti += skip_vt102_codes_non_graph(pti);
		}

		switch (*pti)
		{
			case 27:
				pti += 2;

				if (HAS_BIT(vtc, COL_XTB))
				{
					printf("has XTB\n");
				}

				for (cnt = 0 ; pti[cnt] ; cnt++)
				{
					tmp[cnt] = pti[cnt];

					if (pti[cnt] == ';' || pti[cnt] == 'm')
					{
						tmp[cnt] = 0;

						cnt = -1;
						pti += 1 + strlen(tmp);

						if (HAS_BIT(vtc, COL_256))
						{
							if (HAS_BIT(vtc, COL_XTF))
							{
								fgc = URANGE(0, atoi(tmp), 255);
							}

							if (HAS_BIT(vtc, COL_XTB))
							{
								bgc = URANGE(0, atoi(tmp), 255);
							}
						}
						else
						{
							switch (atoi(tmp))
							{
								case 0:
									vtc = 0;
									fgc = 39;
									bgc = 49;
									break;
								case 1:
									SET_BIT(vtc, COL_BLD);
									break;
								case 5:
									if (HAS_BIT(vtc, COL_XTF) || HAS_BIT(vtc, COL_XTB))
									{
										SET_BIT(vtc, COL_256);
									}
									break;
								case 7:
									SET_BIT(vtc, COL_REV);
									break;
								case  2:
								case 21:
								case 22:
									DEL_BIT(vtc, COL_BLD);
									break;
								case 27:
									DEL_BIT(vtc, COL_REV);
									break;
								case 38:
									DEL_BIT(vtc, COL_XTB);
									SET_BIT(vtc, COL_XTF);
									fgc = 38;
									break;
								case 48:
									DEL_BIT(vtc, COL_XTF);
									SET_BIT(vtc, COL_XTB);
									bgc = 48;
									break;
								default:
									if (atoi(tmp) >= 40 && atoi(tmp) < 50)
									{
										bgc = atoi(tmp);
									}
									if (atoi(tmp) >= 100 && atoi(tmp) < 110)
									{
										bgc = atoi(tmp) - 50;
									}

									if (atoi(tmp) >= 30 && atoi(tmp) < 40)
									{
										fgc = atoi(tmp);
									}
									if (atoi(tmp) >= 90 && atoi(tmp) < 100)
									{
										SET_BIT(vtc, COL_BLD);

										fgc = atoi(tmp) - 60;
									}
									break;
							}
						}
					}

					if (pti[-1] == 'm')
					{
						break;
					}
				}


				if (!HAS_BIT(vtc, COL_REV) && HAS_BIT(ses->vtc, COL_REV))
				{
					cnt = fgc;
					fgc = ses->fgc = bgc - 10;
					bgc = ses->bgc = cnt + 10;
				}

				if (bgc != ses->bgc || fgc != ses->fgc || vtc != ses->vtc)
				{
					sprintf(pto, "</span>");
					pto += strlen(pto);

					if (bgc != ses->bgc)
					{
						if (HAS_BIT(vtc, COL_256) && HAS_BIT(vtc, COL_XTB))
						{
							if (bgc < 8)
							{
								sprintf(pto, "</span><span class='b%d'>", 40+bgc);
							}
							else if (bgc < 16)
							{
								sprintf(pto, "</span><span class='b%d'>", 50+bgc-8);
							}
							else if (bgc < 232)
							{
								sprintf(pto, "</span><span style='background-color: #%c%c%c;'>", x[(bgc-16) / 36], x[(bgc-16) % 36 / 6], x[(bgc-16) % 6]);
							}
							else
							{
								sprintf(pto, "</span><span style='background-color: rgb(%d,%d,%d);'>", (bgc-232) * 10 + 8, (bgc-232) * 10 + 8,(bgc-232) * 10 + 8);
							}
						}
						else
						{
							sprintf(pto, "</span><span class='b%d'>", bgc);
						}
						pto += strlen(pto);
					}

					if (HAS_BIT(vtc, COL_256) && HAS_BIT(vtc, COL_XTF))
					{
						if (fgc < 8)
						{
							sprintf(pto, "</span><span class='d%d'>", 30+fgc);
						}
						else if (fgc < 16)
						{
							sprintf(pto, "</span><span class='l%d'>", 30+fgc-8);
						}
						else if (fgc < 232)
						{
							sprintf(pto, "<span style='color: #%c%c%c;'>", x[(fgc-16) / 36], x[(fgc-16) % 36 / 6], x[(fgc-16) % 6]);
						}
						else
						{
							sprintf(pto, "<span style='color: rgb(%d,%d,%d);'>", (fgc-232) * 10 + 8, (fgc-232) * 10 + 8,(fgc-232) * 10 + 8);
						}
					}
					else
					{
						if (HAS_BIT(vtc, COL_BLD))
						{
							sprintf(pto, "<span class='l%d'>", fgc);
						}
						else
						{
							sprintf(pto, "<span class='d%d'>", fgc);
						}
					}
					pto += strlen(pto);
				}

				if (HAS_BIT(vtc, COL_REV) && !HAS_BIT(ses->vtc, COL_REV))
				{
					cnt = fgc;
					fgc = ses->fgc = bgc - 10;
					bgc = ses->bgc = cnt + 10;
				}
				DEL_BIT(vtc, COL_XTF);
				DEL_BIT(vtc, COL_XTB);
				DEL_BIT(vtc, COL_256);

				ses->vtc = vtc;
				ses->fgc = fgc;
				ses->bgc = bgc;
				break;

			case  6:
				*pto++ = '&';
				pti++;
				break;

			case 28:
				*pto++ = '<';
				pti++;
				break;

			case 30:
				*pto++ = '>';
				pti++;
				break;

			case '>':
				sprintf(pto, "&gt;");
				pto += strlen(pto);
				pti++;
				break;

			case '<':
				sprintf(pto, "&lt;");
				pto += strlen(pto);
				pti++;
				break;

			case '"':
				sprintf(pto, "&quot;");
				pto += strlen(pto);
				pti++;
				break;

			case '&':
				sprintf(pto, "&amp;");
				pto += strlen(pto);
				pti++;
				break;

			case 0:
				break;

			default:
				*pto++ = *pti++;
				break;
		}
	}
	*pto = 0;
}
