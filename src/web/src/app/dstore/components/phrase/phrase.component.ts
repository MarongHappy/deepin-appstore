import { Component, OnInit, Input, Output, OnDestroy, EventEmitter } from '@angular/core';

import { Observable, Subscription, merge, timer, of } from 'rxjs';
import { map, tap, publishReplay, refCount } from 'rxjs/operators';

import { BaseService } from '../../services/base.service';
import { AppService, App } from '../../../services/app.service';
import { SectionPhrase, Section } from '../../services/section';
import { AppFilterFunc, Allowed } from '../appFilter';
import { StoreService } from 'app/modules/client/services/store.service';
import {
  StoreJobInfo,
  StoreJobStatus,
  StoreJobType,
} from 'app/modules/client/models/store-job-info';
import { AppVersion } from 'app/modules/client/models/app-version';
import { JobService } from 'app/services/job.service';

interface AppPhrase {
  app: App;
  phrase: SectionPhrase;
}

@Component({
  selector: 'dstore-phrase',
  templateUrl: './phrase.component.html',
  styleUrls: ['./phrase.component.scss'],
})
export class PhraseComponent implements OnInit, OnDestroy {
  constructor(
    private appService: AppService,
    private storeService: StoreService,
    private jobService: JobService,
  ) {}
  // const
  metadataServer = BaseService.serverHosts.metadataServer;
  isNative = BaseService.isNative;
  StoreJobStatus = StoreJobStatus;
  StoreJobType = StoreJobType;
  @Output()
  loaded = new EventEmitter<Boolean>();
  // input
  @Input()
  section: Section;
  @Input()
  phraseList: SectionPhrase[] = [];
  @Input()
  appFilter: AppFilterFunc = Allowed;

  // data
  more$: Observable<any[]>;
  apps$: Observable<App[]>;
  jobs: { [key: string]: StoreJobInfo } = {};
  jobsNames = new Set<string>();
  jobs$: Subscription;

  // job control
  start = this.storeService.resumeJob;
  pause = this.storeService.pauseJob;
  openApp = this.storeService.openApp;

  ngOnInit() {
    const appNameList = this.phraseList.filter(app => app.show).map(app => app.name);
    this.apps$ = this.appService.getApps(appNameList).pipe(
      tap(() => {
        this.loaded.emit(true);
      }),
      publishReplay(1),
      refCount(),
    );
    this.more$ = this.apps$.pipe(
      map(apps => {
        return ['./apps', { title: this.section.title, apps: apps.map(app => app.name) }];
      }),
    );
    this.getJobs();
  }

  getJobs() {
    this.jobs$ = this.jobService.jobsInfo().subscribe(jobInfos => {
      const jobs = {};
      jobInfos.forEach(job => {
        job.names.forEach(name => {
          jobs[name] = job;
          this.jobsNames.add(name);
        });
      });
      this.jobs = jobs;
    });
  }

  ngOnDestroy() {
    if (this.jobs$) {
      this.jobs$.unsubscribe();
    }
  }
}
