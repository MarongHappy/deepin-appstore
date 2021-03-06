import { Injectable } from '@angular/core';
import { switchMap, map } from 'rxjs/operators';

import { JobService } from 'app/services/job.service';
import { AppService, App } from 'app/services/app.service';
import { StoreService } from 'app/modules/client/services/store.service';
import { InstalledApp } from 'app/modules/client/models/installed';
import { StoreJobType } from 'app/modules/client/models/store-job-info';
import { environment } from 'environments/environment';

@Injectable({
  providedIn: 'root',
})
export class LocalAppService {
  metadataServer = environment.metadataServer;
  constructor(
    private jobService: JobService,
    private storeService: StoreService,
    private appService: AppService,
  ) {}

  LocalAppList() {
    return this.jobService.jobList().pipe(
      switchMap(() => this.storeService.InstalledPackages()),
      switchMap(
        () => this.appService.list(),
        (installed, apps) => {
          const appNameList = apps
            .filter(app => app.packageURI.some(url => installed.has(url)))
            .map(app => app.name);
          return { appNameList, installed };
        },
      ),
      switchMap(
        ({ appNameList }) => this.appService.getApps(appNameList),
        ({ installed }, apps) => {
          apps = apps.sort((a, b) => b.version.installedTime - a.version.installedTime);
          apps.forEach(app => {
            const pkgURL = app.packageURI.find(url => installed.has(url));
            app.version.packageSize = installed.get(pkgURL).size;
          });
          return apps;
        },
      ),
    );
  }

  RemovingList() {
    return this.jobService.jobsInfo().pipe(
      map(jobs => {
        return jobs
          .filter(job => job.type === StoreJobType.uninstall)
          .map(job => job.names)
          .reduce((acc, names) => {
            acc.push(...names);
            return acc;
          }, []);
      }),
    );
  }
  RemoveLocalApp(app: App) {
    this.storeService.removePackages([app]);
  }
}
