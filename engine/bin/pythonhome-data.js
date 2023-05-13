
  var Module = typeof Module !== 'undefined' ? Module : {};
  
  if (!Module.expectedDataFileDownloads) {
    Module.expectedDataFileDownloads = 0;
  }
  Module.expectedDataFileDownloads++;
  (function() {
   var loadPackage = function(metadata) {
  
      var PACKAGE_PATH;
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.toString().substring(0, window.location.pathname.toString().lastIndexOf('/')) + '/');
      } else if (typeof location !== 'undefined') {
        // worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.toString().substring(0, location.pathname.toString().lastIndexOf('/')) + '/');
      } else {
        throw 'using preloaded data can only be done on a web page or in a web worker';
      }
      var PACKAGE_NAME = 'pythonhome.data';
      var REMOTE_PACKAGE_BASE = 'pythonhome.data';
      if (typeof Module['locateFilePackage'] === 'function' && !Module['locateFile']) {
        Module['locateFile'] = Module['locateFilePackage'];
        err('warning: you defined Module.locateFilePackage, that has been renamed to Module.locateFile (using your locateFilePackage for now)');
      }
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
    
      var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];
      var PACKAGE_UUID = metadata['package_uuid'];
    
      function fetchRemotePackage(packageName, packageSize, callback, errback) {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', packageName, true);
        xhr.responseType = 'arraybuffer';
        xhr.onprogress = function(event) {
          var url = packageName;
          var size = packageSize;
          if (event.total) size = event.total;
          if (event.loaded) {
            if (!xhr.addedTotal) {
              xhr.addedTotal = true;
              if (!Module.dataFileDownloads) Module.dataFileDownloads = {};
              Module.dataFileDownloads[url] = {
                loaded: event.loaded,
                total: size
              };
            } else {
              Module.dataFileDownloads[url].loaded = event.loaded;
            }
            var total = 0;
            var loaded = 0;
            var num = 0;
            for (var download in Module.dataFileDownloads) {
            var data = Module.dataFileDownloads[download];
              total += data.total;
              loaded += data.loaded;
              num++;
            }
            total = Math.ceil(total * Module.expectedDataFileDownloads/num);
            if (Module['setStatus']) Module['setStatus']('Downloading data... (' + loaded + '/' + total + ')');
          } else if (!Module.dataFileDownloads) {
            if (Module['setStatus']) Module['setStatus']('Downloading data...');
          }
        };
        xhr.onerror = function(event) {
          throw new Error("NetworkError for: " + packageName);
        }
        xhr.onload = function(event) {
          if (xhr.status == 200 || xhr.status == 304 || xhr.status == 206 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            var packageData = xhr.response;
            callback(packageData);
          } else {
            throw new Error(xhr.statusText + " : " + xhr.responseURL);
          }
        };
        xhr.send(null);
      };

      function handleError(error) {
        console.error('package error:', error);
      };
    
    function runWithFS() {
  
      function assert(check, msg) {
        if (!check) throw msg + new Error().stack;
      }
  Module['FS_createPath']('/', 'lib', true, true);
Module['FS_createPath']('/lib', 'python2.7', true, true);
Module['FS_createPath']('/lib/python2.7', 'json', true, true);
Module['FS_createPath']('/lib/python2.7', 'importlib', true, true);
Module['FS_createPath']('/lib/python2.7', 'encodings', true, true);
Module['FS_createPath']('/lib/python2.7', 'xml', true, true);
Module['FS_createPath']('/lib/python2.7/xml', 'etree', true, true);

      /** @constructor */
      function DataRequest(start, end, audio) {
        this.start = start;
        this.end = end;
        this.audio = audio;
      }
      DataRequest.prototype = {
        requests: {},
        open: function(mode, name) {
          this.name = name;
          this.requests[name] = this;
          Module['addRunDependency']('fp ' + this.name);
        },
        send: function() {},
        onload: function() {
          var byteArray = this.byteArray.subarray(this.start, this.end);
          this.finish(byteArray);
        },
        finish: function(byteArray) {
          var that = this;
  
          Module['FS_createDataFile'](this.name, null, byteArray, true, true, true); // canOwn this data in the filesystem, it is a slide into the heap that will never change
          Module['removeRunDependency']('fp ' + that.name);
  
          this.requests[this.name] = null;
        }
      };
  
          var files = metadata['files'];
          for (var i = 0; i < files.length; ++i) {
            new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio']).open('GET', files[i]['filename']);
          }
  
    
        var indexedDB;
        if (typeof window === 'object') {
          indexedDB = window.indexedDB || window.mozIndexedDB || window.webkitIndexedDB || window.msIndexedDB;
        } else if (typeof location !== 'undefined') {
          // worker
          indexedDB = self.indexedDB;
        } else {
          throw 'using IndexedDB to cache data can only be done on a web page or in a web worker';
        }
        var IDB_RO = "readonly";
        var IDB_RW = "readwrite";
        var DB_NAME = "EM_PRELOAD_CACHE";
        var DB_VERSION = 1;
        var METADATA_STORE_NAME = 'METADATA';
        var PACKAGE_STORE_NAME = 'PACKAGES';
        function openDatabase(callback, errback) {
          try {
            var openRequest = indexedDB.open(DB_NAME, DB_VERSION);
          } catch (e) {
            return errback(e);
          }
          openRequest.onupgradeneeded = function(event) {
            var db = event.target.result;

            if(db.objectStoreNames.contains(PACKAGE_STORE_NAME)) {
              db.deleteObjectStore(PACKAGE_STORE_NAME);
            }
            var packages = db.createObjectStore(PACKAGE_STORE_NAME);

            if(db.objectStoreNames.contains(METADATA_STORE_NAME)) {
              db.deleteObjectStore(METADATA_STORE_NAME);
            }
            var metadata = db.createObjectStore(METADATA_STORE_NAME);
          };
          openRequest.onsuccess = function(event) {
            var db = event.target.result;
            callback(db);
          };
          openRequest.onerror = function(error) {
            errback(error);
          };
        };

        // This is needed as chromium has a limit on per-entry files in IndexedDB
        // https://cs.chromium.org/chromium/src/content/renderer/indexed_db/webidbdatabase_impl.cc?type=cs&sq=package:chromium&g=0&l=177
        // https://cs.chromium.org/chromium/src/out/Debug/gen/third_party/blink/public/mojom/indexeddb/indexeddb.mojom.h?type=cs&sq=package:chromium&g=0&l=60
        // We set the chunk size to 64MB to stay well-below the limit
        var CHUNK_SIZE = 64 * 1024 * 1024;

        function cacheRemotePackage(
          db,
          packageName,
          packageData,
          packageMeta,
          callback,
          errback
        ) {
          var transactionPackages = db.transaction([PACKAGE_STORE_NAME], IDB_RW);
          var packages = transactionPackages.objectStore(PACKAGE_STORE_NAME);
          var chunkSliceStart = 0;
          var nextChunkSliceStart = 0;
          var chunkCount = Math.ceil(packageData.byteLength / CHUNK_SIZE);
          var finishedChunks = 0;
          for (var chunkId = 0; chunkId < chunkCount; chunkId++) {
            nextChunkSliceStart += CHUNK_SIZE;
            var putPackageRequest = packages.put(
              packageData.slice(chunkSliceStart, nextChunkSliceStart),
              'package/' + packageName + '/' + chunkId
            );
            chunkSliceStart = nextChunkSliceStart;
            putPackageRequest.onsuccess = function(event) {
              finishedChunks++;
              if (finishedChunks == chunkCount) {
                var transaction_metadata = db.transaction(
                  [METADATA_STORE_NAME],
                  IDB_RW
                );
                var metadata = transaction_metadata.objectStore(METADATA_STORE_NAME);
                var putMetadataRequest = metadata.put(
                  {
                    'uuid': packageMeta.uuid,
                    'chunkCount': chunkCount
                  },
                  'metadata/' + packageName
                );
                putMetadataRequest.onsuccess = function(event) {
                  callback(packageData);
                };
                putMetadataRequest.onerror = function(error) {
                  errback(error);
                };
              }
            };
            putPackageRequest.onerror = function(error) {
              errback(error);
            };
          }
        }

        /* Check if there's a cached package, and if so whether it's the latest available */
        function checkCachedPackage(db, packageName, callback, errback) {
          var transaction = db.transaction([METADATA_STORE_NAME], IDB_RO);
          var metadata = transaction.objectStore(METADATA_STORE_NAME);
          var getRequest = metadata.get('metadata/' + packageName);
          getRequest.onsuccess = function(event) {
            var result = event.target.result;
            if (!result) {
              return callback(false, null);
            } else {
              return callback(PACKAGE_UUID === result['uuid'], result);
            }
          };
          getRequest.onerror = function(error) {
            errback(error);
          };
        }

        function fetchCachedPackage(db, packageName, metadata, callback, errback) {
          var transaction = db.transaction([PACKAGE_STORE_NAME], IDB_RO);
          var packages = transaction.objectStore(PACKAGE_STORE_NAME);

          var chunksDone = 0;
          var totalSize = 0;
          var chunkCount = metadata['chunkCount'];
          var chunks = new Array(chunkCount);

          for (var chunkId = 0; chunkId < chunkCount; chunkId++) {
            var getRequest = packages.get('package/' + packageName + '/' + chunkId);
            getRequest.onsuccess = function(event) {
              // If there's only 1 chunk, there's nothing to concatenate it with so we can just return it now
              if (chunkCount == 1) {
                callback(event.target.result);
              } else {
                chunksDone++;
                totalSize += event.target.result.byteLength;
                chunks.push(event.target.result);
                if (chunksDone == chunkCount) {
                  if (chunksDone == 1) {
                    callback(event.target.result);
                  } else {
                    var tempTyped = new Uint8Array(totalSize);
                    var byteOffset = 0;
                    for (var chunkId in chunks) {
                      var buffer = chunks[chunkId];
                      tempTyped.set(new Uint8Array(buffer), byteOffset);
                      byteOffset += buffer.byteLength;
                      buffer = undefined;
                    }
                    chunks = undefined;
                    callback(tempTyped.buffer);
                    tempTyped = undefined;
                  }
                }
              }
            };
            getRequest.onerror = function(error) {
              errback(error);
            };
          }
        }
      
      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer instanceof ArrayBuffer, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        
          // Reuse the bytearray from the XHR as the source for file reads.
          DataRequest.prototype.byteArray = byteArray;
    
            var files = metadata['files'];
            for (var i = 0; i < files.length; ++i) {
              DataRequest.prototype.requests[files[i].filename].onload();
            }
                Module['removeRunDependency']('datafile_pythonhome.data');

      };
      Module['addRunDependency']('datafile_pythonhome.data');
    
      if (!Module.preloadResults) Module.preloadResults = {};
    
        function preloadFallback(error) {
          console.error(error);
          console.error('falling back to default preload behavior');
          fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, processPackageData, handleError);
        };

        openDatabase(
          function(db) {
            checkCachedPackage(db, PACKAGE_PATH + PACKAGE_NAME,
              function(useCached, metadata) {
                Module.preloadResults[PACKAGE_NAME] = {fromCache: useCached};
                if (useCached) {
                  fetchCachedPackage(db, PACKAGE_PATH + PACKAGE_NAME, metadata, processPackageData, preloadFallback);
                } else {
                  fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE,
                    function(packageData) {
                      cacheRemotePackage(db, PACKAGE_PATH + PACKAGE_NAME, packageData, {uuid:PACKAGE_UUID}, processPackageData,
                        function(error) {
                          console.error(error);
                          processPackageData(packageData);
                        });
                    }
                  , preloadFallback);
                }
              }
            , preloadFallback);
          }
        , preloadFallback);

        if (Module['setStatus']) Module['setStatus']('Downloading...');
      
    }
    if (Module['calledRun']) {
      runWithFS();
    } else {
      if (!Module['preRun']) Module['preRun'] = [];
      Module["preRun"].push(runWithFS); // FS is not initialized yet, wait for it
    }
  
   }
   loadPackage({"files": [{"filename": "/lib/python2.7/fnmatch.pyo", "start": 0, "end": 2347, "audio": 0}, {"filename": "/lib/python2.7/keyword.pyo", "start": 2347, "end": 4164, "audio": 0}, {"filename": "/lib/python2.7/ast.pyo", "start": 4164, "end": 11349, "audio": 0}, {"filename": "/lib/python2.7/io.pyo", "start": 11349, "end": 13417, "audio": 0}, {"filename": "/lib/python2.7/locale.pyo", "start": 13417, "end": 65221, "audio": 0}, {"filename": "/lib/python2.7/__future__.pyo", "start": 65221, "end": 67421, "audio": 0}, {"filename": "/lib/python2.7/collections.pyo", "start": 67421, "end": 84926, "audio": 0}, {"filename": "/lib/python2.7/colorsys.pyo", "start": 84926, "end": 88258, "audio": 0}, {"filename": "/lib/python2.7/dummy_thread.pyo", "start": 88258, "end": 91335, "audio": 0}, {"filename": "/lib/python2.7/tarfile.pyo", "start": 91335, "end": 151929, "audio": 0}, {"filename": "/lib/python2.7/linecache.pyo", "start": 151929, "end": 154566, "audio": 0}, {"filename": "/lib/python2.7/_abcoll.pyo", "start": 154566, "end": 174760, "audio": 0}, {"filename": "/lib/python2.7/genericpath.pyo", "start": 174760, "end": 177356, "audio": 0}, {"filename": "/lib/python2.7/shutil.pyo", "start": 177356, "end": 190817, "audio": 0}, {"filename": "/lib/python2.7/types.pyo", "start": 190817, "end": 193336, "audio": 0}, {"filename": "/lib/python2.7/hashlib.pyo", "start": 193336, "end": 199686, "audio": 0}, {"filename": "/lib/python2.7/abc.pyo", "start": 199686, "end": 203490, "audio": 0}, {"filename": "/lib/python2.7/UserDict.pyo", "start": 203490, "end": 212835, "audio": 0}, {"filename": "/lib/python2.7/traceback.pyo", "start": 212835, "end": 220265, "audio": 0}, {"filename": "/lib/python2.7/StringIO.pyo", "start": 220265, "end": 226559, "audio": 0}, {"filename": "/lib/python2.7/codecs.pyo", "start": 226559, "end": 246693, "audio": 0}, {"filename": "/lib/python2.7/difflib.pyo", "start": 246693, "end": 274326, "audio": 0}, {"filename": "/lib/python2.7/site.pyo", "start": 274326, "end": 287813, "audio": 0}, {"filename": "/lib/python2.7/urlparse.pyo", "start": 287813, "end": 298793, "audio": 0}, {"filename": "/lib/python2.7/functools.pyo", "start": 298793, "end": 304128, "audio": 0}, {"filename": "/lib/python2.7/gettext.pyo", "start": 304128, "end": 321156, "audio": 0}, {"filename": "/lib/python2.7/posixpath.pyo", "start": 321156, "end": 329760, "audio": 0}, {"filename": "/lib/python2.7/platform.pyo", "start": 329760, "end": 357530, "audio": 0}, {"filename": "/lib/python2.7/re.pyo", "start": 357530, "end": 364101, "audio": 0}, {"filename": "/lib/python2.7/_sysconfigdata.pyo", "start": 364101, "end": 364245, "audio": 0}, {"filename": "/lib/python2.7/_weakrefset.pyo", "start": 364245, "end": 373635, "audio": 0}, {"filename": "/lib/python2.7/sysconfig.pyo", "start": 373635, "end": 388519, "audio": 0}, {"filename": "/lib/python2.7/os.pyo", "start": 388519, "end": 403518, "audio": 0}, {"filename": "/lib/python2.7/dis.pyo", "start": 403518, "end": 409124, "audio": 0}, {"filename": "/lib/python2.7/sre_parse.pyo", "start": 409124, "end": 430008, "audio": 0}, {"filename": "/lib/python2.7/random.pyo", "start": 430008, "end": 446911, "audio": 0}, {"filename": "/lib/python2.7/copy_reg.pyo", "start": 446911, "end": 451270, "audio": 0}, {"filename": "/lib/python2.7/zipfile.pyo", "start": 451270, "end": 487892, "audio": 0}, {"filename": "/lib/python2.7/sre_compile.pyo", "start": 487892, "end": 500150, "audio": 0}, {"filename": "/lib/python2.7/contextlib.pyo", "start": 500150, "end": 503154, "audio": 0}, {"filename": "/lib/python2.7/tempfile.pyo", "start": 503154, "end": 518082, "audio": 0}, {"filename": "/lib/python2.7/sre_constants.pyo", "start": 518082, "end": 524191, "audio": 0}, {"filename": "/lib/python2.7/pickle.pyo", "start": 524191, "end": 557418, "audio": 0}, {"filename": "/lib/python2.7/string.pyo", "start": 557418, "end": 570106, "audio": 0}, {"filename": "/lib/python2.7/heapq.pyo", "start": 570106, "end": 581592, "audio": 0}, {"filename": "/lib/python2.7/warnings.pyo", "start": 581592, "end": 591795, "audio": 0}, {"filename": "/lib/python2.7/tokenize.pyo", "start": 591795, "end": 603042, "audio": 0}, {"filename": "/lib/python2.7/weakref.pyo", "start": 603042, "end": 616494, "audio": 0}, {"filename": "/lib/python2.7/token.pyo", "start": 616494, "end": 620222, "audio": 0}, {"filename": "/lib/python2.7/inspect.pyo", "start": 620222, "end": 645132, "audio": 0}, {"filename": "/lib/python2.7/struct.pyo", "start": 645132, "end": 645365, "audio": 0}, {"filename": "/lib/python2.7/stat.pyo", "start": 645365, "end": 647920, "audio": 0}, {"filename": "/lib/python2.7/textwrap.pyo", "start": 647920, "end": 653512, "audio": 0}, {"filename": "/lib/python2.7/argparse.pyo", "start": 653512, "end": 708120, "audio": 0}, {"filename": "/lib/python2.7/copy.pyo", "start": 708120, "end": 717715, "audio": 0}, {"filename": "/lib/python2.7/glob.pyo", "start": 717715, "end": 720011, "audio": 0}, {"filename": "/lib/python2.7/opcode.pyo", "start": 720011, "end": 725980, "audio": 0}, {"filename": "/lib/python2.7/json/scanner.pyo", "start": 725980, "end": 728147, "audio": 0}, {"filename": "/lib/python2.7/json/__init__.pyo", "start": 728147, "end": 730923, "audio": 0}, {"filename": "/lib/python2.7/json/encoder.pyo", "start": 730923, "end": 739726, "audio": 0}, {"filename": "/lib/python2.7/json/decoder.pyo", "start": 739726, "end": 747381, "audio": 0}, {"filename": "/lib/python2.7/importlib/__init__.pyo", "start": 747381, "end": 748511, "audio": 0}, {"filename": "/lib/python2.7/encodings/ascii.pyo", "start": 748511, "end": 750578, "audio": 0}, {"filename": "/lib/python2.7/encodings/utf_32_be.pyo", "start": 750578, "end": 752386, "audio": 0}, {"filename": "/lib/python2.7/encodings/latin_1.pyo", "start": 752386, "end": 754481, "audio": 0}, {"filename": "/lib/python2.7/encodings/hex_codec.pyo", "start": 754481, "end": 757055, "audio": 0}, {"filename": "/lib/python2.7/encodings/utf_8.pyo", "start": 757055, "end": 758819, "audio": 0}, {"filename": "/lib/python2.7/encodings/zlib_codec.pyo", "start": 758819, "end": 762264, "audio": 0}, {"filename": "/lib/python2.7/encodings/__init__.pyo", "start": 762264, "end": 765085, "audio": 0}, {"filename": "/lib/python2.7/encodings/aliases.pyo", "start": 765085, "end": 773252, "audio": 0}, {"filename": "/lib/python2.7/xml/__init__.pyo", "start": 773252, "end": 773796, "audio": 0}, {"filename": "/lib/python2.7/xml/etree/ElementPath.pyo", "start": 773796, "end": 781264, "audio": 0}, {"filename": "/lib/python2.7/xml/etree/__init__.pyo", "start": 781264, "end": 781387, "audio": 0}, {"filename": "/lib/python2.7/xml/etree/ElementTree.pyo", "start": 781387, "end": 815494, "audio": 0}], "remote_package_size": 815494, "package_uuid": "e3811d5c-d75f-4d0e-80cb-4b6caed82b81"});
  
  })();
  