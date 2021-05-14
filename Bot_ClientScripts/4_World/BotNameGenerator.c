//Scripted bi Kolobov
// Edited bi VasiliyP
class BotNameGenerator
{
	static ref array<string> m_NameS = new array<string>;
	
	static void BotNameGeneratorInsert()
	{
		m_NameS.Insert("Bob");
		m_NameS.Insert("Putin");
		m_NameS.Insert("Nick");
		m_NameS.Insert("Ali");
		m_NameS.Insert("Vasiliy");
		m_NameS.Insert("Pasha");
		m_NameS.Insert("Serg");
		m_NameS.Insert("Dron");
		m_NameS.Insert("Comic");
		m_NameS.Insert("Piter");
		m_NameS.Insert("Tetris");
		m_NameS.Insert("Trump");
		m_NameS.Insert("Gavno");
		m_NameS.Insert("Mocha");
		m_NameS.Insert("Elda");
		m_NameS.Insert("Evgen");
		m_NameS.Insert("Terminator");
		m_NameS.Insert("Enot");
		m_NameS.Insert("Taran");
		m_NameS.Insert("Gaber");
		m_NameS.Insert("Mike");
		m_NameS.Insert("Sherdan");
		m_NameS.Insert("Fedya");
		m_NameS.Insert("Philip");
		m_NameS.Insert("Paul");
		m_NameS.Insert("Hren");
		m_NameS.Insert("Pipec");
		m_NameS.Insert("Georg");
		m_NameS.Insert("Lion");
		m_NameS.Insert("Goobers");
		m_NameS.Insert("AnGeL");
		m_NameS.Insert("Anarhian");
		m_NameS.Insert("Cherry");
		m_NameS.Insert("Bumagorez");
		m_NameS.Insert("JM_Listener");
		m_NameS.Insert("Bezumie");
		m_NameS.Insert("OveRBanG");
		m_NameS.Insert("SinRin");
		m_NameS.Insert("Tuk");
		m_NameS.Insert("Black_Star");
		m_NameS.Insert("Arhangel");
		m_NameS.Insert("Nagibatou");
		m_NameS.Insert("MrSmileGod");
		m_NameS.Insert("KoTuk 400kg");
		m_NameS.Insert("Mr_R.I.P");
		m_NameS.Insert("DONwerewolf");
		m_NameS.Insert("GrandX");
		m_NameS.Insert("MaXiMEN");
		m_NameS.Insert("NeZoX");
		m_NameS.Insert("Sortir");
		m_NameS.Insert("Bloodray");
		m_NameS.Insert("WOGY");
	}
	
	static string GetNameRandom()
	{
		return m_NameS.Get(Math.RandomInt(0,m_NameS.Count()));
	}
};