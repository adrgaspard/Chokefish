using CommunityToolkit.Mvvm.ComponentModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace AdrGaspard.ChokefishSuite.MVVM
{
    public class BotVsHumanMatchViewModel : MatchViewModelBase
    {
        public override bool MatchCanStart => true;

        protected override void OnMatchStopped()
        {
            throw new NotImplementedException();
        }

        protected override void OnReset()
        {
            throw new NotImplementedException();
        }

        protected override void StartMatchProcedure(CancellationToken token)
        {
            throw new NotImplementedException();
        }
    }
}
