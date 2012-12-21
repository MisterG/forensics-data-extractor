/**
 * Project: forensics-data-extractor
 * File name: fde-console.cpp
 * Description: the main function of the GUI console
 *
 * @author Mathieu Grzybek on 2012-05-20
 * @copyright 2012 Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * forensics-data-extractor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <QtGui/QApplication>
#include "gui/main_window.h"

int	main(int argc, char *argv[])
{
	QCoreApplication::setOrganizationName("Forensics-Data-Extractor");
	QCoreApplication::setApplicationName("Qt-Console");

	zmq::context_t zmq_context(1);
	QApplication a(argc, argv);

	Main_Window w((void*)&zmq_context);
	w.show();

	return a.exec();
}

