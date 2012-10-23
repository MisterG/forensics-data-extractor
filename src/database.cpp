#include "database.h"

Database::Database(const QString& db_file) {
	db = QSqlDatabase::addDatabase("QSQLITE", db_file);
	db.setDatabaseName(db_file);

	// TODO: throw an exception
	if ( db.open() == false ) {
		qCritical() << "Cannot connect to the database " << db_file;
	}

	if ( init_schema() == false )
		throw "init failed";
}

Database::~Database() {
	// We never unlock the mutex because the object is destroyed
	mutex.lock();

	QString connection_name = db.databaseName();
	db.close();

	QSqlDatabase::removeDatabase(connection_name);
}

bool	Database::exec(const QString& query) {
	mutex.lock();

	if ( db.transaction() == false ) {
		qCritical() << "Transaction begin failed: " << db.lastError().text();
		db.rollback();
		qCritical() << "Rollback!";
		mutex.unlock();
		return false;
	}

	if ( atomic_exec(query) == false ) {
		db.rollback();
		qCritical() << "Rollback!";
		mutex.unlock();
		return false;
	}

	if ( db.commit() == false ) {
		qCritical() << "Commit failed: " << db.lastError().text();
		db.rollback();
		qCritical() << "Rollback!";
		mutex.unlock();
		return  false;
	}

	mutex.unlock();
	return true;
}

bool	Database::exec(const QStringList& queries) {
	mutex.lock();

	if ( db.transaction() == false ) {
		qCritical() << "Transaction begin failed: " << db.lastError().text();
		db.rollback();
		mutex.unlock();
		return false;
	}

	Q_FOREACH(QString query, queries) {
		if ( atomic_exec(query) == false ) {
			db.rollback();
			qCritical() << "Rollback!";
			mutex.unlock();
			return false;
		}
	}

	if ( db.commit() == false ) {
		qCritical() << "Commit failed: " << db.lastError().text();
		db.rollback();
		qCritical() << "Rollback!";
		mutex.unlock();
		return  false;
	}

	mutex.unlock();
	return true;
}

QSqlDatabase*	Database::get_db() {
	return &db;
}

uint	Database::get_row_count(const QString& table_name) {
	QString		query;
	QSqlQuery	q(db);

	query = "SELECT COUNT(*) FROM " % table_name % ";";
	q.exec(query);

	if ( q.next() == true )
		return q.value(0).toUInt();

	return 0;
}

bool	Database::init_schema() {
	QStringList	queries;

	queries << "CREATE TABLE IF NOT EXISTS place (site TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS cookie (name TEXT PRIMARY KEY, value TEXT NOT NULL, host TEXT NOT NULL, path TEXT NOT NULL, expiration TEXT NOT NULL, secured TEXT NOT NULL, http TEXT NOT NULL, last_accessed TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS download (name TEXT PRIMARY KEY, source TEXT NOT NULL, mime TEXT NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS form (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL);";
	//queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, value TEXT UNIQUE NOT NULL, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS search (name TEXT PRIMARY KEY, hits INTEGER NOT NULL);";
	queries << "CREATE TABLE IF NOT EXISTS signon (host TEXT PRIMARY KEY, id TEXT NOT NULL, password TEXT NOT NULL );";
	queries << "CREATE TABLE IF NOT EXISTS parsed_file (file TEXT PRIMARY KEY, md5 TEXT, sha1 TEXT, analysed INTEGER DEFAULT '0');";

	queries << "CREATE VIEW IF NOT EXISTS analysed_file AS SELECT file FROM parsed_file WHERE analysed = 1";

	return exec(queries);
}

bool	Database::atomic_exec(const QString& query) {
	QSqlQuery	q(db);

	if ( q.exec(query) == false ) {
		qCritical() << "Atomic exec failed on" << query << " " << q.lastError().text();
		return false;
	}

	qDebug() << "query OK: " << query;
	return true;
}

bool	Database::insert_file(const struct_file& file) {
	QString	query = "INSERT INTO parsed_file (file, md5, sha1) VALUES ('";

	query += file.full_path % "','";
	query += (char*) file.md5;
	query += "','";
	query += (char*) file.sha1;
	query += "');";

	printf("sha1: %s and md5: %s\n", file.sha1, file.md5);
	return exec(query);
}
