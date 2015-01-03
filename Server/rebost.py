import urllib2
import json
import sys


def print_html(versions):

		basepath = "http://www.softcatala.org/pub/softcatala"
		rebostid = "5643"
		
		for v in versions:
				os = "win32"
				fileout = "linkrebost.html"
				if v == 'OSX':
						os = "mac"
						fileout = "linkrebost-mac.html"
				outcome = "http://baixades.softcatala.org/?url="+basepath+versions[v]['path']+"&id="+rebostid+"&mirall=catalanitzadorp&so="+os+"&versio="+versions[v]['version']
				with open( fileout, 'w') as file_:
						file_.write(outcome)


def main(argv):

		response = urllib2.urlopen('http://www.softcatala.org/w/api.php?action=ask&query=%5B%5BProgram::Rebost:Catalanitzador_de_Softcatal%C3%A0%5D%5D|%3FHas%20OS|%3FHas%20Version|%3FHas%20Download%20Local|mainlabel=-&format=json')
		data = json.load(response)

		versions = dict()

		if data is not None:
				if 'query' in data:
						if 'results' in data['query']:
								results = data['query']['results']
								for r in results:
										if 'printouts' in results[r]:
												out = results[r]['printouts']
												if out['Has OS'] is not None:
														os = out['Has OS'][0]
														if out['Has Version'] is not None:
																version = out['Has Version'][0]
																if out['Has Download Local'] is not None:
																		versions[ os ] = dict()
																		versions[ os ]['version'] = version
																		versions[ os ]['path'] = out['Has Download Local'][0]

		if len( versions ) > 0:
			print_html( versions )

if __name__ == "__main__":
		main(sys.argv[1:])

