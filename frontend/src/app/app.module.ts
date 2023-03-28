import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { MdbModalModule } from 'mdb-angular-ui-kit/modal';

import {
  FaIconLibrary,
  FontAwesomeModule,
} from '@fortawesome/angular-fontawesome';
import { far } from '@fortawesome/free-regular-svg-icons';
import { fas } from '@fortawesome/free-solid-svg-icons';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';
import { ParametersComponent } from './parameters/parameters.component';
import { ParameterComponent } from './parameter/parameter.component';
import { TypeModalComponent } from './type-modal/type-modal.component';
import { EnumSearchComponent } from './enum-search/enum-search.component';

@NgModule({
  declarations: [
    AppComponent,
    ParametersComponent,
    ParameterComponent,
    TypeModalComponent,
    EnumSearchComponent,
  ],
  imports: [BrowserModule, AppRoutingModule, FontAwesomeModule, MdbModalModule],
  providers: [],
  bootstrap: [AppComponent],
})
export class AppModule {
  constructor(library: FaIconLibrary) {
    // Add multiple icons to the library
    library.addIconPacks(far, fas);
  }
}
