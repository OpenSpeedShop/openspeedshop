import OSSParser
import pySS

def rptidargs( args ):
    if args.has_key('host'):
        host=pySS.Host(args['host'])
    else:
        host=pySS.Host('localhost')
    # pid
    if args.has_key('pid'):
        # and a thread
        if args.has_key('tid'):
            return pySS.PosixThread(args['pid'], args['tid'], host)
        else:
            return pySS.Process( args['pid'], host)
    # a tid, but no pid (because of previous case)
    if args.has_key('tid'):
        # FIXME: We should have a knowledge of the focus
        return pySS.PosixThread(-1, args['tid'], host)
    #
    # rank
    if args.has_key('rank'):
        if args['rank'].has_key('file'):
            file=pySS.File(args['rank']['file'], host )
        else:
            file=pySS.File('<unknwon_file>', host )
        if args['rank'].has_key('rid'):
            return pySS.Rank( file, args['rank']['rid'] )
    #
    #
    # There's an error if we get up to there
    return None


def expAttachCmd( cmd ):
    exp = pySS.Experiment("just a try")
    if cmd['expAttach'].has_key( 'rptid'):
        thread=rptidargs( cmd['expAttach']['rptid'] )
        # print "type(thread)=", type(thread)
        if type(thread) is pySS.PosixThread:
            exp.attachToPosixThread( thread )
        else:
            if type(thread) is pySS.Process:
                exp.attachToProcess( thread )
            else:
                if type(thread) is pySS.Rank:
                    exp.attachToMPIRank( thread )
                else:
                    print "Unknwon type for thread: ", type( thread )


def expCreateCmd( cmd ):
    args=cmd['expCreate']
    # 
    if args.has_key('host'):
        host=pySS.Host(args['host'])
    else:
        host=pySS.Host('localhost')
    #
    if args.has_key( 'file'):
        file=pySS.File( cmd['expCreate']['file'], host )
    else:
        file=pySS.File( "<unknwon file>", host )
    #
    if args.has_key( 'expType' ):
        exp=pySS.Experiment( file, args['expType'])
        exp.describe()



def listPidsCmd( cmd ):
    args=cmd['listPids']
    host=pySS.Host( 'localhost')
    if type(args) is dict:
        if args.has_key('host'):
            host=pySS.Host( args['host'])
    lpids= pySS.listPidsOnHost( host )
    print lpids

def semantic( cmd ):
    if cmd.has_key('listPids'):
        listPidsCmd( cmd )
    if cmd.has_key('expAttach'):
        expAttachCmd( cmd )
    if cmd.has_key('expCreate'):
        expCreateCmd( cmd )


